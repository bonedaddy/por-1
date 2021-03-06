#include "Server.h"
#include <iostream>
#include <time.h>
#include "naccache_stern.h"

Server::Server(Field & F, size_t m, size_t n, Givaro::Integer & p):
F(F), _m(m), _n(n), _p(p) {
    
}

int Server::send(Field::Element_ptr M, Givaro::Modular<Givaro::Integer>::Element_ptr w, Givaro::Integer & n) {
    this->M = M;
    this->w = w;
    this->cipherSize = Givaro::Integer(n);
}


Server::Field::Element_ptr Server::audit(Field::Element r, Givaro::Integer & delta) {
    Field::Element_ptr x = FFLAS::fflas_new(F, 1, _m); 
    clock_t t1 = clock();
    F.init(x[0], r);
    for(size_t i = 1; i < _m; i++) {
        Field::Element tmp;
        F.mul(tmp, x[i-1], r);
        F.init(x[i],tmp);
    }
    clock_t t2 = clock();
    std::cout << "time for x : " << (t2 - t1)/(CLOCKS_PER_SEC) << std::endl;
    Field::Element_ptr y = FFLAS::fflas_new(F, 1, _n);
    t1 = clock();
    for (size_t i = 0; i < _m; i++) {
        Givaro::Integer xi;
        F.convert(xi, x[i]);
        if(x[i] < 0) {
            delta = SMCStrassen::add_ciphers(delta, SMCStrassen::mul_ciphers(w[i], xi + _p, cipherSize), cipherSize);
        } else { 
            delta = SMCStrassen::add_ciphers(delta, SMCStrassen::mul_ciphers(w[i], xi,cipherSize), cipherSize);
        }
    }
    t2 = clock();
    std::cout << "time for delta : " << (t2 - t1)/(CLOCKS_PER_SEC) << std::endl;
    t1 = clock();
    FFLAS::fgemm(F, FFLAS::FflasNoTrans, FFLAS::FflasNoTrans, 1, _n, _m, F.one, x, _m, M, _n, F.zero, y, _n);
    t2 = clock();
    std::cout << "time for y : " << (t2 - t1)/(CLOCKS_PER_SEC) << std::endl;

    return y;
}
