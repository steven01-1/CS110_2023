#### Senario 1:

1.每一步是8个word，4*8= 32bytes, 正好跳了一个cache的大小，每次必miss。
2.每次必miss，改变循环次不影响结果，每次做的都是同样的事情。
3.step设为1, option改为1。 3/4 的访问hit。如果read miss，紧接着的write 一定hit。

#### Senario 2:

1. 每一步是2*4 = 8bytes, 先read miss 然后write 一定hit，之后访问8bytes后的word，还在这一条block内。3/4 = 75%
2. 趋近于100%， cache满后全都hit. $ \frac {n - 16 }{n} \to  1, n \to \infty $ . 
3. Cache Blocking Techniques . 将array分块成[0, 256], [257, 512] ...

#### Senario 3:

1. [0, 50%]
2. Associativity -> 1。第一循环全不中，第二次循环全中。