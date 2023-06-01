#include "d2q9_bgk.h"
#include "emmintrin.h"
#include "immintrin.h"

/* The main processes in one step */
int collision(const t_param params, t_speed* cells, t_speed* tmp_cells, char* obstacles);
int streaming(const t_param params, t_speed* cells, t_speed* tmp_cells);
int obstacle(const t_param params, t_speed* cells, t_speed* tmp_cells, char* obstacles);
int boundary(const t_param params, t_speed* cells, t_speed* tmp_cells, float* inlets);

/*
** The main calculation methods.
** timestep calls, in order, the functions:
** collision(), obstacle(), streaming() & boundary()
*/
int timestep(const t_param params, t_speed* cells, t_speed* tmp_cells, float* inlets, char* obstacles)
{
  /* The main time overhead, you should mainly optimize these processes. */
  collision(params, cells, tmp_cells, obstacles);
//  obstacle(params, cells, tmp_cells, obstacles);
//  streaming(params, cells, tmp_cells);
  boundary(params, cells, tmp_cells, inlets);
  return EXIT_SUCCESS;
}

/*
** The collision of fluids in the cell is calculated using 
** the local equilibrium distribution and relaxation process
*/
int collision(const t_param params, t_speed* cells, t_speed* tmp_cells, char* obstacles) {
  const float c_sq = 1.f / 3.f; /* square of speed of sound */
  const float w0 = 4.f / 9.f; /* weighting factor */
  const float w1 = 1.f / 9.f; /* weighting factor */
  const float w2 = 1.f / 36.f; /* weighting factor */

  /* loop over the cells in the grid
  ** the collision step is called before
  ** the streaming step and so values of interest
  ** are in the scratch-space grid */
#pragma omp parallel
  {
    int start_row = -1, end_row = -1;
    // 1: It has set start_row
    int flag = 0;
  #pragma omp for
  for (int jj = 0; jj < params.ny; ++jj) {
    if (!flag) {
      start_row = jj;
      flag = 1;
    }
    end_row = jj;
    for (int ii = 0; ii < params.nx; ii++) {
      // obstacle
      __builtin_prefetch(&obstacles[jj * params.nx + ii + 2], 0, 2);
      if (obstacles[jj * params.nx + ii]) {
        /* called after collision, so taking values from scratch space
        ** mirroring, and writing into main grid */
        float cells0 = cells[ii + jj * params.nx].speeds[0];
        float cells1 = cells[ii + jj * params.nx].speeds[1];
        float cells2 = cells[ii + jj * params.nx].speeds[2];
        float cells3 = cells[ii + jj * params.nx].speeds[3];
        float cells4 = cells[ii + jj * params.nx].speeds[4];
        float cells5 = cells[ii + jj * params.nx].speeds[5];
        float cells6 = cells[ii + jj * params.nx].speeds[6];
        float cells7 = cells[ii + jj * params.nx].speeds[7];
        float cells8 = cells[ii + jj * params.nx].speeds[8];
        tmp_cells[ii + jj * params.nx].speeds[0] = cells0;
        tmp_cells[ii + jj * params.nx].speeds[1] = cells3;
        tmp_cells[ii + jj * params.nx].speeds[2] = cells4;
        tmp_cells[ii + jj * params.nx].speeds[3] = cells1;
        tmp_cells[ii + jj * params.nx].speeds[4] = cells2;
        tmp_cells[ii + jj * params.nx].speeds[5] = cells7;
        tmp_cells[ii + jj * params.nx].speeds[6] = cells8;
        tmp_cells[ii + jj * params.nx].speeds[7] = cells5;
        tmp_cells[ii + jj * params.nx].speeds[8] = cells6;

//        tmp_cells[ii + jj * params.nx].speeds[0] = cells[ii + jj * params.nx].speeds[0];
//        tmp_cells[ii + jj * params.nx].speeds[1] = cells[ii + jj * params.nx].speeds[3];
//        tmp_cells[ii + jj * params.nx].speeds[2] = cells[ii + jj * params.nx].speeds[4];
//        tmp_cells[ii + jj * params.nx].speeds[3] = cells[ii + jj * params.nx].speeds[1];
//        tmp_cells[ii + jj * params.nx].speeds[4] = cells[ii + jj * params.nx].speeds[2];
//        tmp_cells[ii + jj * params.nx].speeds[5] = cells[ii + jj * params.nx].speeds[7];
//        tmp_cells[ii + jj * params.nx].speeds[6] = cells[ii + jj * params.nx].speeds[8];
//        tmp_cells[ii + jj * params.nx].speeds[7] = cells[ii + jj * params.nx].speeds[5];
//        tmp_cells[ii + jj * params.nx].speeds[8] = cells[ii + jj * params.nx].speeds[6];
      } // end for if
      // collision
      else {
        /* compute local density total */

//        float local_density = cells[ii + jj * params.nx].speeds[0], local_d1 = 0.f;
//        for (int kk = 1; kk < NSPEEDS; kk += 2) {
//          local_density += cells[ii + jj * params.nx].speeds[kk];
//          local_d1 += cells[ii + jj* params.nx].speeds[kk + 1];
//        }
//        local_density += local_d1;

//        float local_density = cells[ii + jj * params.nx].speeds[0],
//              tmp1 = cells[ii + jj * params.nx].speeds[2],
//              tmp2 = cells[ii + jj * params.nx].speeds[3];
//        local_density += cells[ii + jj * params.nx].speeds[1] + tmp1 + tmp2;
//        float tmp3 = cells[ii + jj * params.nx].speeds[4],
//              tmp4 = cells[ii + jj * params.nx].speeds[5],
//              tmp5 = cells[ii + jj * params.nx].speeds[6];
//        local_density += tmp3 + tmp4 + tmp5 +
//                         cells[ii + jj * params.nx].speeds[7]
//                         + cells[ii + jj * params.nx].speeds[8];

        float local_density = cells[ii + jj * params.nx].speeds[0];
        float local1 = cells[ii + jj * params.nx].speeds[1];
        float local2 = cells[ii + jj * params.nx].speeds[2];
        float local3 = cells[ii + jj * params.nx].speeds[3];
        float local4 = cells[ii + jj * params.nx].speeds[4];
        float local5 = cells[ii + jj * params.nx].speeds[5];
        float local6 = cells[ii + jj * params.nx].speeds[6];
        float local7 = cells[ii + jj * params.nx].speeds[7];
        float local8 = cells[ii + jj * params.nx].speeds[8];
        local_density += local1 + local2 + local3 + local4 + local5 + local6 + local7 + local8;


        float u_x = cells[ii + jj * params.nx].speeds[1] - cells[ii + jj * params.nx].speeds[3];
        float u_y = cells[ii + jj * params.nx].speeds[2] - cells[ii + jj * params.nx].speeds[4];
        u_x += cells[ii + jj * params.nx].speeds[5] - cells[ii + jj * params.nx].speeds[6];
        u_y += cells[ii + jj * params.nx].speeds[5] + cells[ii + jj * params.nx].speeds[6];
        u_x += - cells[ii + jj * params.nx].speeds[7] + cells[ii + jj * params.nx].speeds[8];
        u_y += - cells[ii + jj * params.nx].speeds[7] - cells[ii + jj * params.nx].speeds[8];
        // local_density^(-1)
        float local_density_1 = 1.f / local_density;
        u_x *= local_density_1;
        u_y *= local_density_1;

        /* velocity squared */
        float u_sq = u_x * u_x + u_y * u_y;

        /* directional velocity components */
        float u[NSPEEDS];
        u[0] = 0; /* zero */
        u[1] = u_x; /* east */
        u[2] = u_y; /* north */
        u[3] = -u_x; /* west */
        u[4] = -u_y; /* south */
        u[5] = u_x + u_y; /* north-east */
        u[6] = -u_x + u_y; /* north-west */
        u[7] = -u_x - u_y; /* south-west */
        u[8] = u_x - u_y; /* south-east */

        /* equilibrium densities */
        float d_equ[NSPEEDS];
        /* zero velocity density: weight w0 */

        d_equ[0] = w0 * local_density * (1.f - u_sq / (2.f * c_sq));
        tmp_cells[ii + jj * params.nx].speeds[0] =
        cells[ii + jj * params.nx].speeds[0]
        + params.omega * (d_equ[0] - cells[ii + jj * params.nx].speeds[0]);

        /* axis speeds: weight w1 */

        __m128 u1 = _mm_loadu_ps(&u[1]), CSQ = _mm_set1_ps(c_sq), CSQ_1 = _mm_set1_ps(1.f / c_sq);
        __m128 d_equ_w1_vec = _mm_mul_ps(_mm_add_ps(u1, CSQ), CSQ_1);
        d_equ_w1_vec =  _mm_add_ps(d_equ_w1_vec, _mm_mul_ps(_mm_sub_ps(_mm_mul_ps(u1, u1), _mm_set1_ps(u_sq * c_sq)),
                                            _mm_set1_ps(1.f/ (2.f * c_sq * c_sq))));
        d_equ_w1_vec = _mm_mul_ps(d_equ_w1_vec, _mm_set1_ps(w1 * local_density * params.omega));
        // store
        d_equ_w1_vec = _mm_add_ps(
        d_equ_w1_vec, _mm_mul_ps(_mm_set1_ps(1.f - params.omega),
                                 _mm_loadu_ps((float *)&cells[ii + jj * params.nx].speeds[1])));
        _mm_storeu_ps((float *)&tmp_cells[ii + jj * params.nx].speeds[1], d_equ_w1_vec);



        __m128 u5 = _mm_loadu_ps(&u[5]);
        __m128 d_equ_w2_vec = _mm_mul_ps(_mm_add_ps(u5, CSQ), CSQ_1);
        d_equ_w2_vec =  _mm_add_ps(d_equ_w2_vec, _mm_mul_ps(_mm_sub_ps(_mm_mul_ps(u5, u5), _mm_set1_ps(u_sq * c_sq)),
                                            _mm_set1_ps(1.f/ (2.f * c_sq * c_sq))));
        d_equ_w2_vec = _mm_mul_ps(d_equ_w2_vec, _mm_set1_ps(w2 * local_density * params.omega));
//      store
        d_equ_w2_vec = _mm_add_ps(d_equ_w2_vec, _mm_mul_ps(_mm_set1_ps(1.f - params.omega),
                                _mm_loadu_ps((float *) &cells[ii + jj*params.nx].speeds[5])));
        _mm_storeu_ps((float *) &tmp_cells[ii + jj*params.nx].speeds[5], d_equ_w2_vec);


//        float c1 = local_density * params.omega * w1,
//        c2 = local_density * params.omega * w2;
//        __m256 u1_256 = _mm256_loadu_ps(&u[1]), CSQ256 = _mm256_set1_ps(c_sq),
//               CSQ_1_256 = _mm256_set1_ps(1.f / c_sq);
//        __m256 coefficient = _mm256_set_ps(c2, c2, c2, c2, c1, c1, c1, c1);
//        __m256 d_equ256 = _mm256_mul_ps(_mm256_add_ps(u1_256, CSQ256), CSQ_1_256);
//        d_equ256 = _mm256_add_ps(d_equ256, _mm256_mul_ps(_mm256_sub_ps(_mm256_mul_ps(u1_256, u1_256), _mm256_set1_ps(u_sq * c_sq)),
//                                            _mm256_set1_ps(1.f/ (2.f * c_sq * c_sq))));
//        d_equ256 = _mm256_mul_ps(d_equ256, coefficient);
////             store
//        d_equ256 = _mm256_add_ps(d_equ256, _mm256_mul_ps(_mm256_set1_ps(1.f - params.omega),
//                                _mm256_loadu_ps((float *) &cells[ii + jj*params.nx].speeds[1])));
//        _mm256_storeu_ps((float *) &tmp_cells[ii + jj*params.nx].speeds[1], d_equ256);


      } // end of else

      /* streaming */
      // cells speed[0], center
      cells[ii + jj * params.nx].speeds[0] = tmp_cells[ii + jj * params.nx].speeds[0];
      //  cells speed[1], east
      if (ii > 0) {
        cells[ii  + jj * params.nx].speeds[1] = tmp_cells[ii - 1 + jj * params.nx].speeds[1];
      }
      // cells speed[3], west
      if (ii > 0) {
        cells[ii - 1 + jj * params.nx].speeds[3] = tmp_cells[ii + jj * params.nx].speeds[3];
      }
      // cells speed[2] speed[6] speed[5] north, northwest, northeast
      if (jj > start_row) {
        cells[ii + jj * params.nx].speeds[2] = tmp_cells[ii + (jj - 1) * params.nx].speeds[2];
        cells[ii + jj * params.nx].speeds[6] =
        tmp_cells[((ii >= params.nx) ? 0 : (ii + 1)) + (jj - 1) * params.nx].speeds[6];
        cells[ii + jj * params.nx].speeds[5] =
        tmp_cells[((ii == 0) ? (params.nx - 1) : (ii - 1)) + (jj - 1) * params.nx].speeds[5];
      }
      //     cells speed[4] speed[7] speed[8] south, southwest, southeast
      if (jj > start_row) {
        cells[ii + (jj - 1) * params.nx].speeds[4] = tmp_cells[ii + jj * params.nx].speeds[4];
        if (ii != 0) {
          cells[ii - 1 + (jj - 1) * params.nx].speeds[7] 
            = tmp_cells[ii + jj * params.nx].speeds[7];
          cells[ii + (jj - 1) * params.nx].speeds[8] = tmp_cells[ii - 1 + jj * params.nx].speeds[8];
        }
        if (ii == params.nx - 1) {
          cells[ii + (jj - 1) * params.nx].speeds[7] = tmp_cells[jj * params.nx].speeds[7];
          cells[(jj - 1) * params.nx].speeds[8] = tmp_cells[ii + jj * params.nx].speeds[8];
        }
      }
    } // end of loop over _all_ cells for ii

    /* streaming */
    //  cells speed[1], east (0, jj)
    cells[0 + jj * params.nx].speeds[1] = tmp_cells[params.nx - 1 + jj * params.nx].speeds[1];
    // cells speed[3], west (0, jj)
    cells[params.nx - 1 + jj * params.nx].speeds[3] = tmp_cells[0 + jj * params.nx].speeds[3];
//     cells speed[4] speed[7] speed[8] south, southwest, southeast
/*
    if (jj > start_row) {
      __builtin_prefetch(&cells[0 + (jj - 1) * params.nx].speeds[7], 1);
      __builtin_prefetch(&tmp_cells[1 + jj * params.nx].speeds[7], 0);
      for (int i1 = 0; i1 < params.nx; i1 += 2) {
        cells[i1 + (jj - 1) * params.nx].speeds[4] = tmp_cells[i1 + jj * params.nx].speeds[4];
        cells[i1 + (jj - 1) * params.nx].speeds[7] =
        tmp_cells[i1 + 1 + jj * params.nx].speeds[7];
        cells[i1 + (jj - 1) * params.nx].speeds[8] =
        tmp_cells[((i1 == 0) ? (params.nx - 1) : (i1 - 1)) + jj * params.nx].speeds[8];

        cells[i1 + 1 + (jj - 1) * params.nx].speeds[4] = tmp_cells[i1 + 1 + jj * params.nx].speeds[4];
        cells[i1 + 1 + (jj - 1) * params.nx].speeds[7] =
        tmp_cells[((i1 + 1 >= params.nx) ? 0 : (i1 + 2)) + jj * params.nx].speeds[7];
        cells[i1 + 1 + (jj - 1) * params.nx].speeds[8] =
        tmp_cells[i1 + jj * params.nx].speeds[8];

//        __builtin_prefetch(&tmp_cells[i1 + 3 + jj * params.nx].speeds[7], 0);
      }
    }
    */
  } // end of loop over _all_ cells for jj
//     update end_row and start_row
# pragma omp barrier
  for (int i1 = 0; i1 < params.nx; ++i1) {
    // end_row
    cells[i1 + end_row * params.nx].speeds[4] =
    tmp_cells[i1 + ((end_row >= params.ny) ? 0 : (end_row + 1)) * params.nx].speeds[4];
    cells[i1 + end_row * params.nx].speeds[7] =
    tmp_cells[((i1 >= params.nx) ? 0 : (i1 + 1)) + ((end_row >= params.ny) ? 0 : (end_row + 1)) * params.nx].speeds[7];
    cells[i1 + end_row * params.nx].speeds[8] =
    tmp_cells[((i1 == 0) ? (params.nx - 1) : (i1 - 1))
              + ((end_row >= params.ny) ? 0 : (end_row + 1)) * params.nx].speeds[8];
    // start_row
    cells[i1 + start_row * params.nx].speeds[2] =
    tmp_cells[i1 + ((start_row == 0) ? (params.ny - 1) : (start_row - 1)) * params.nx].speeds[2];
    cells[i1 + start_row * params.nx].speeds[6] =
    tmp_cells[((i1 >= params.nx) ? 0 : (i1 + 1)) +
              ((start_row == 0) ? (params.ny - 1) : (start_row - 1)) * params.nx].speeds[6];
    cells[i1 + start_row * params.nx].speeds[5] =
    tmp_cells[((i1 == 0) ? (params.nx - 1) : (i1 - 1)) +
              ((start_row == 0) ? (params.ny - 1) : (start_row - 1)) * params.nx].speeds[5];
  }
} // end of parallel
  return EXIT_SUCCESS;
} // end of collision()

/*
** For obstacles, mirror their speed.
*/
int obstacle(const t_param params, t_speed* cells, t_speed* tmp_cells, char* obstacles) {

//  /* loop over the cells in the grid */
//  #pragma omp parallel for 
//  for (int jj = 0; jj < params.ny; jj++)
//  {
//    for (int ii = 0; ii < params.nx; ii++)
//    {
//      /* if the cell contains an obstacle */
//      if (obstacles[jj*params.nx + ii])
//      {
//        /* called after collision, so taking values from scratch space
//        ** mirroring, and writing into main grid */
//        tmp_cells[ii + jj*params.nx].speeds[0] = cells[ii + jj*params.nx].speeds[0];
//        tmp_cells[ii + jj*params.nx].speeds[1] = cells[ii + jj*params.nx].speeds[3];
//        tmp_cells[ii + jj*params.nx].speeds[2] = cells[ii + jj*params.nx].speeds[4];
//        tmp_cells[ii + jj*params.nx].speeds[3] = cells[ii + jj*params.nx].speeds[1];
//        tmp_cells[ii + jj*params.nx].speeds[4] = cells[ii + jj*params.nx].speeds[2];
//        tmp_cells[ii + jj*params.nx].speeds[5] = cells[ii + jj*params.nx].speeds[7];
//        tmp_cells[ii + jj*params.nx].speeds[6] = cells[ii + jj*params.nx].speeds[8];
//        tmp_cells[ii + jj*params.nx].speeds[7] = cells[ii + jj*params.nx].speeds[5];
//        tmp_cells[ii + jj*params.nx].speeds[8] = cells[ii + jj*params.nx].speeds[6];
//      }
//    }
//  }
  return EXIT_SUCCESS;
}

/*
** Particles flow to the corresponding cell according to their speed direaction.
*/
int streaming(const t_param params, t_speed* cells, t_speed* tmp_cells) {
/* loop over _all_ cells */

//  #pragma omp parallel for 
////  for (int jj = 0; jj < params.ny; jj++)
//  for (int jj = params.ny - 1; jj >= 0; --jj)
//  {
//    for (int ii = 0; ii < params.nx; ii++)
//    {
//      /* determine indices of axis-direction neighbours
//      ** respecting periodic boundary conditions (wrap around) */
//      int y_n = (jj + 1) % params.ny;
//      int x_e = (ii + 1) % params.nx;
//      int y_s = (jj == 0) ? (params.ny - 1) : (jj - 1);
//      int x_w = (ii == 0) ? (params.nx - 1) : (ii - 1);
//      /* propagate densities from neighbouring cells, following
//      ** appropriate directions of travel and writing into
//      ** scratch space grid */
//     // L2 cache??
////      cells[ii  + jj *params.nx].speeds[0] = tmp_cells[ii + jj*params.nx].speeds[0]; /* central cell, no movement */
////      cells[x_e + jj *params.nx].speeds[1] = tmp_cells[ii + jj*params.nx].speeds[1]; /* east */
//      cells[ii  + y_n*params.nx].speeds[2] = tmp_cells[ii + jj*params.nx].speeds[2]; /* north */
////      cells[x_w + jj *params.nx].speeds[3] = tmp_cells[ii + jj*params.nx].speeds[3]; /* west */
//      cells[ii  + y_s*params.nx].speeds[4] = tmp_cells[ii + jj*params.nx].speeds[4]; /* south */
//      cells[x_e + y_n*params.nx].speeds[5] = tmp_cells[ii + jj*params.nx].speeds[5]; /* north-east */
//      cells[x_w + y_n*params.nx].speeds[6] = tmp_cells[ii + jj*params.nx].speeds[6]; /* north-west */
//      cells[x_w + y_s*params.nx].speeds[7] = tmp_cells[ii + jj*params.nx].speeds[7]; /* south-west */
//      cells[x_e + y_s*params.nx].speeds[8] = tmp_cells[ii + jj*params.nx].speeds[8]; /* south-east */
//
//    } // end of loop over for ii
//  }

  return EXIT_SUCCESS;
}

/*
** Work with boundary conditions. The upper and lower boundaries use the rebound plane, 
** the left border is the inlet of fixed speed, and 
** the right border is the open outlet of the first-order approximation.
*/
int boundary(const t_param params, t_speed* cells,  t_speed* tmp_cells, float* inlets) {
  /* Set the constant coefficient */
  const float cst1 = 2.0/3.0;
  const float cst2 = 1.0/6.0;
  const float cst3 = 1.0/2.0;

  int ii, jj, iii;
  float local_density;


  jj = params.ny -1;

  #pragma omp parallel for 
  for(ii = 0; ii < params.nx; ii++){
//     bottom wall (bounce) jj = 0
    cells[ii].speeds[2] = tmp_cells[ii].speeds[4];
    cells[ii].speeds[5] = tmp_cells[ii].speeds[7];
    cells[ii].speeds[6] = tmp_cells[ii].speeds[8];
//     top wall (bounce)
    cells[ii + jj*params.nx].speeds[4] = tmp_cells[ii + jj*params.nx].speeds[2];
    cells[ii + jj*params.nx].speeds[7] = tmp_cells[ii + jj*params.nx].speeds[5];
    cells[ii + jj*params.nx].speeds[8] = tmp_cells[ii + jj*params.nx].speeds[6];
  }


//  ii = 0;
//  iii = params.nx-1;
//  for(jj = 0; jj < params.ny; jj++){
//    // left wall (inlet)
//    float s0 = cells[jj*params.nx].speeds[0];
//    float s1 = cells[jj*params.nx].speeds[1];
//    float s2 = cells[jj*params.nx].speeds[2];
//    float s3 = cells[jj*params.nx].speeds[3];
//    float s4 = cells[jj*params.nx].speeds[4];
//    float s6 = cells[jj*params.nx].speeds[6];
//    float s7 = cells[jj*params.nx].speeds[7];
//    local_density = ( s0
//                      + s1
//                      + s4
//                      + 2.f * (s3 + s6 + s7)
//                      )/(1.f - inlets[jj]);
//
//    cells[jj*params.nx].speeds[1] = s3
//                                        + cst1*local_density*inlets[jj];
//
//    cells[jj*params.nx].speeds[5] = s7
//                                        - cst3*(s2- s4)
//                                        + cst2*local_density*inlets[jj];
//
//    cells[jj*params.nx].speeds[8] = s6
//                                        + cst3*(s2 - s4)
//                                        + cst2*local_density*inlets[jj];
//
////    local_density = ( cells[ii + jj*params.nx].speeds[0]
////                     + cells[ii + jj*params.nx].speeds[2]
////                     + cells[ii + jj*params.nx].speeds[4]
////                     + 2.0 * cells[ii + jj*params.nx].speeds[3]
////                     + 2.0 * cells[ii + jj*params.nx].speeds[6]
////                     + 2.0 * cells[ii + jj*params.nx].speeds[7]
////                     )/(1.0 - inlets[jj]);
////
////    cells[ii + jj*params.nx].speeds[1] = cells[ii + jj*params.nx].speeds[3]
////                                           + cst1*local_density*inlets[jj];
////
////    cells[ii + jj*params.nx].speeds[5] = cells[ii + jj*params.nx].speeds[7]
////                                           - cst3*(cells[ii + jj*params.nx].speeds[2]-cells[ii + jj*params.nx].speeds[4])
////                                           + cst2*local_density*inlets[jj];
////
////    cells[ii + jj*params.nx].speeds[8] = cells[ii + jj*params.nx].speeds[6]
////                                           + cst3*(cells[ii + jj*params.nx].speeds[2]-cells[ii + jj*params.nx].speeds[4])
////                                           + cst2*local_density*inlets[jj];
//
//    // right wall (outlet)
//    for (int kk = 0; kk < NSPEEDS; kk++) {
//      cells[params.nx - 1 + jj * params.nx].speeds[kk] = cells[params.nx - 2 + jj * params.nx].speeds[kk];
//    }
//  }

//    ii = 0;
  iii = params.nx-1;
  for(jj = 0; jj < params.ny; jj++){
    // left wall (inlet)
//    local_density = ( cells[jj*params.nx].speeds[0]
//                     + cells[jj*params.nx].speeds[2]
//                     + cells[jj*params.nx].speeds[4]
//                     + 2.0 * cells[jj*params.nx].speeds[3]
//                     + 2.0 * cells[jj*params.nx].speeds[6]
//                     + 2.0 * cells[jj*params.nx].speeds[7]
//                     )/(1.0 - inlets[jj]);

        local_density = ( cells[jj*params.nx].speeds[0]
                         + cells[jj*params.nx].speeds[2]
                         + cells[jj*params.nx].speeds[4]
                         + 2.0 * (cells[jj*params.nx].speeds[3]
                         + cells[jj*params.nx].speeds[6]
                         +  cells[jj*params.nx].speeds[7])
                         )/(1.0 - inlets[jj]);

    cells[jj*params.nx].speeds[1] = cells[jj*params.nx].speeds[3]
                                      + cst1*local_density*inlets[jj];

    cells[jj*params.nx].speeds[5] = cells[jj*params.nx].speeds[7]
                                      - cst3*(cells[jj*params.nx].speeds[2]-cells[jj*params.nx].speeds[4])
                                      + cst2*local_density*inlets[jj];

    cells[jj*params.nx].speeds[8] = cells[jj*params.nx].speeds[6]
                                      + cst3*(cells[jj*params.nx].speeds[2]-cells[jj*params.nx].speeds[4])
                                      + cst2*local_density*inlets[jj];
    // right wall (outlet)
    for (int kk = 0; kk < NSPEEDS; kk++) {
      cells[iii + jj * params.nx].speeds[kk] = cells[iii - 1 + jj * params.nx].speeds[kk];
    }
  }

  return EXIT_SUCCESS;
}
