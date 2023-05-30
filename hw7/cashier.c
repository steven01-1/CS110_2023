#include "cashier.h"
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>

/**
 *
 * @param x : exponent
 * @return log_2 (x)
 */
uint64_t Log2(uint64_t x) {
  uint64_t res = 0;
  while (x >>= 1) {
    res++;
  }
  return res;
}


/**
 *
 * @param config : config information
 * @return a new cashier
 */
struct cashier *cashier_init(struct cache_config config) {
  // allocate memory for new cache
  struct cashier * new_cache = (struct cashier *)malloc(sizeof(struct cashier));
  // check if malloc is successful
  if (new_cache == NULL) {
    return NULL;
  }
  // initialize the cache config
  new_cache->config = config;
  // initialize size
  new_cache->size = new_cache->config.line_size * new_cache->config.lines;
  // allocate memory for cache lines
  new_cache->lines = (struct cache_line*) malloc(
  sizeof(struct cache_line) * new_cache->config.lines);
  // check if malloc is successful
  if (new_cache->lines == NULL) {
    free(new_cache);
    return NULL;
  }
  // initialize the each cache line
  for (uint64_t i = 0; i < new_cache->config.lines; ++i) {
    new_cache->lines[i].valid = 0;
    new_cache->lines[i].dirty = 0;
    new_cache->lines[i].tag = 0;
    new_cache->lines[i].last_access = 0;
    // for data
    new_cache->lines[i].data = (uint8_t*) calloc(new_cache->config.line_size, sizeof(uint8_t));
    // check if malloc is failed.
    if (new_cache->lines[i].data == NULL) {
      // release all cache lines' memory
      for (uint64_t j = 0; j < i; ++j) {
        free(new_cache->lines[j].data);
      }
      // free cache_line* lines
      free(new_cache->lines);
      free(new_cache);
      return NULL;
    }
  } // end of for loop
  // initialize masks and bits info
  // offset
  new_cache->offset_bits = Log2(new_cache->config.line_size);
  // offset mask. It is used before bit calculation.
  assert(new_cache->offset_bits < new_cache->config.address_bits);
  new_cache->offset_mask = (1 << new_cache->offset_bits) - 1;
  // index
  new_cache->index_bits = Log2(new_cache->config.lines / new_cache->config.ways);
  assert(new_cache->index_bits < new_cache->config.address_bits);
  // index mask. It is used before bit calculation.
  new_cache->index_mask = ((1 << new_cache->index_bits) - 1)
                          << new_cache->offset_bits;
  // tag
  new_cache->tag_bits = new_cache->config.address_bits - new_cache->offset_bits
                        - new_cache->index_bits;
  assert(new_cache->tag_bits + new_cache->index_bits
  <= new_cache->config.address_bits);
  // tag mask. It is used before bit calculation.
  new_cache->tag_mask = ~(new_cache->tag_mask + new_cache->index_mask);
  // success
  return new_cache;
}

void cashier_release(struct cashier *cache) {
  // check if cache is NULL
  if (!cache) {
    return;
  }
  // check blocks one by one.
  // way
  for (uint64_t i = 0; i < cache->config.ways; ++i) {
    // set
    for (uint64_t j = 0; j < (cache->config.lines / cache->config.ways); ++j) {
      // dirty and valid
      uint64_t line_id = j * cache->config.ways + i;
      if (cache->lines[line_id].valid) {
        // routine
        before_eviction(j, &cache->lines[line_id]);
      if (cache->lines[line_id].dirty) {
        // write data
        for (uint64_t k = 0; k < cache->config.line_size; ++k) {
          uint64_t address =
          k + (j << cache->offset_bits)
          + (cache->lines[line_id].tag << (cache->config.address_bits - cache->tag_bits));
          mem_write(address, cache->lines[line_id].data[k]);
        } // end for a line's data
      } // end for a line
    }
      free(cache->lines[line_id].data);
    } // set
  } // way
  free(cache->lines);
  free(cache);
} // end for release

bool cashier_read(struct cashier *cache, uint64_t addr, uint8_t *byte) {
  // extract information
  uint64_t set_index = (addr & cache->index_mask) >> cache->offset_bits;
  uint64_t tag = (addr & cache->tag_mask) >> (cache->config.address_bits - cache->tag_bits);
  uint64_t offset = addr & cache->offset_mask;

  bool find_invalid = false;
  uint64_t LRU_time= ULONG_MAX;
  // target line
  uint64_t target = ULONG_MAX;
  // iterate in a set
  for (uint64_t i = 0; i < cache->config.ways; ++i) {
    uint64_t line_id = set_index * cache->config.ways + i;
    // hit
    if (cache->lines[line_id].valid && cache->lines[line_id].tag == tag) {
      // read data to cache
      *byte = cache->lines[line_id].data[offset];
      // update timestamp
      cache->lines[line_id].last_access = get_timestamp();
      return true;
    }

    // find a place to load memory
    if (!find_invalid) {
      // find an invalid line
      if (!cache->lines[line_id].valid) {
        target = line_id;
        find_invalid = true;
      }
      // find an earlier line
      else if (cache->lines[line_id].last_access < LRU_time) {
        LRU_time = cache->lines[line_id].last_access;
        target = line_id;
      }
    } // end for searching invalid line
  } // end for iteration over a set

  // miss
  // get the victim line
  struct cache_line *target_line = &cache->lines[target];
  // not evict, invalid
  if (find_invalid) {
    // read data
    for (uint64_t i = 0; i < cache->config.line_size; ++i) {
      uint64_t addr_base = addr & (~cache->offset_mask);
      target_line->data[i] = mem_read(addr_base + i);
    }
  }
  // evict
  else {
  // routine
    before_eviction(set_index, target_line);
    // dirty cache line
    if (target_line->dirty) {
      // write back and read
      uint64_t base_addr = (target_line->tag << (cache->config.address_bits - cache->tag_bits))
                           + (set_index << cache->offset_bits);
      for (uint64_t i = 0; i < cache->config.line_size; ++i) {
        mem_write(base_addr + i, target_line->data[i]);
      }
      // read from memory
      for (uint64_t i = 0; i < cache->config.line_size; ++i) {
        target_line->data[i] = mem_read((addr & (~cache->offset_mask)) + i);
      }
    } // end for dirty eviction
    // not dirty
    else {
      // read data
      uint64_t addr_base = addr & (~cache->offset_mask);
      for (uint64_t i = 0; i < cache->config.line_size; ++i) {
        target_line->data[i] = mem_read(addr_base + i);
      }
    } // end for not dirty eviction
  } // end for eviction
  // common part
  target_line->tag = tag;
  target_line->valid = true;
  target_line->last_access = get_timestamp();
  target_line->dirty = false;
  // read data to buffer
  *byte = target_line->data[offset];
  return false;
}

bool cashier_write(struct cashier *cache, uint64_t addr, uint8_t byte) {
  // extract information
  uint64_t set_index = (addr & cache->index_mask) >> cache->offset_bits;
  uint64_t tag = (addr & cache->tag_mask) >> (cache->config.address_bits - cache->tag_bits);
  uint64_t offset = addr & cache->offset_mask;
  // note for a target line
  bool find_invalid = false;
  uint64_t LRU_time= ULONG_MAX;
  // target line
  uint64_t target = ULONG_MAX;
  // iterate in a set
  for (uint64_t i = 0; i < cache->config.ways; ++i) {
    uint64_t line_id = set_index * cache->config.ways + i;
    // hit
    if (cache->lines[line_id].valid && cache->lines[line_id].tag == tag) {
      // write data to buffer
      cache->lines[line_id].data[offset] = byte;
      // update timestamp
      cache->lines[line_id].last_access = get_timestamp();
      // set dirty
      cache->lines[line_id].dirty = true;
      return true;
    } // end for hit

    // find a place to write
    if (!find_invalid) {
      // find an invalid line
      if (!cache->lines[line_id].valid) {
        target = line_id;
        find_invalid = true;
      }
      // find an earlier line
      else if (cache->lines[line_id].last_access < LRU_time) {
        LRU_time = cache->lines[line_id].last_access;
        target = line_id;
      }
    }
  } // end for iteration in a set

  // load memory and write
  struct cache_line *target_line = &cache->lines[target];
  // invalid target line

  // evict, write back
  if (target_line->dirty) {
    // routine
    before_eviction(set_index, target_line);
    uint64_t base_addr = (target_line->tag << (cache->config.address_bits - cache->tag_bits))
                         + (set_index << cache->offset_bits);
    for (uint64_t i = 0; i < cache->config.line_size; ++i) {
      mem_write(base_addr + i, target_line->data[i]);
    }
  }

  // common part
  // read data
  for (uint64_t i = 0; i < cache->config.line_size; ++i) {
    target_line->data[i] = mem_read((addr & (~cache->offset_mask)) + i);
  }
  // write to buffer
  target_line->data[offset] = byte;
  target_line->dirty = true;
  target_line->valid = true;
  target_line->tag = tag;
  target_line->last_access = get_timestamp();
  return false;
}
