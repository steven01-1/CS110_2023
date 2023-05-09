_mm_load_si128: 要求内存地址对齐，否则报错。
_mm_loadu_si128：地址不用对齐。

_mm_load_pd：读入两个64-bit的double，要求内存地址对齐。
_mm_load1_pd：把一个64-bit数加载到两段. des[0:63]，des[64:127]

### 3.

modify:

a0是j，O3直接用的，没有存入stack再取出来。

访问堆变量`a`更简洁，地址高20为直接加载到a4，且不覆写（O0）覆写了。

`main`函数里面O3没有跑`for`循环，直接把a要加的数一下算出来了。

```
modify:
        addi    sp,sp,-32
        sw      s0,28(sp)
        addi    s0,sp,32
        sw      a0,-20(s0) # 把j 存stack了
        lui     a5,%hi(a) # 加载a的地址
        lw      a4,%lo(a)(a5)
        lw      a5,-20(s0) # 把a的地址覆写了
        add     a4,a4,a5
        lui     a5,%hi(a) 
        sw      a4,%lo(a)(a5)
        nop
        lw      s0,28(sp)
        addi    sp,sp,32
        jr      ra
```

