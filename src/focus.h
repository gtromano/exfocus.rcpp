#ifndef ___FCOST_H___
#define ___FCOST_H___

#include <iostream>
#include <list>
#include <numeric>      // std::iota
#include <cmath>
#include <algorithm>
#include <functional>

struct CUSUM {
  double Sn = 0.0;
  int n = 0.0;
};


// basic piece function
struct Piece {
  double St = 0.0;
  int tau = 0;
  double m0 = 0.0;
  double Mdiff = 0.0;

  double eval (const CUSUM& cs, double x, const double& theta0) const {
    auto c = (double)(cs.n - tau);
    auto S = (cs.Sn - St);

    std::cout << "running default" << std::endl;

    if (std::isnan(theta0))
      return 0.0;
    else
      return 0.0;

  }

};

struct PieceGau:Piece {
  double eval (const CUSUM& cs, double x, const double& theta0) const {
    auto c = (double)(cs.n - tau);
    auto S = (cs.Sn - St);

    std::cout << "running gaussian" << std::endl;

    if (std::isnan(theta0))
      return - 0.5 * c * x * x + S * x + m0;
    else
      return - 0.5 * c * x * x + S * x;

  }
};

struct PieceBer:Piece {
  double eval (const CUSUM& cs, double x, const double& theta0) const {
    auto c = (double)(cs.n - tau);
    auto S = (cs.Sn - St);

    std::cout << "running bernoulli" << std::endl;

    if (std::isnan(theta0))
      return S * log(x) + (c - S) * log((1-x)) + m0;
    else
      return S * log(x/theta0) + (c - S) * log((1-x) / (1 - theta0));

  }
};

struct PiecePoi:Piece {

};

struct PieceGam:Piece {
  double shape = 1.0;
};

template <typename T> struct Cost {
  std::list<T> ps;
  double opt = 0;
};

template <typename T> struct Info {
  CUSUM cs;
  Cost<T> Ql;
  Cost<T> Qr;
};


// double argmax (const Piece &q, const CUSUM &cs );
// double argmax (const PieceGam &q, const CUSUM &cs );
//
// double eval (const PieceGau &q, const CUSUM &cs, double x, const double &theta0);
// double eval (const PieceBer &q, const CUSUM &cs, double x, const double &theta0);
// void prune (Cost &Q, const CUSUM &cs, const double &theta0, std::function<bool(Piece, Piece)> cond);
auto focus_step (auto I, const double& y, auto newP, const double& thres, const double& theta0, const bool& adp_max_check);

#endif
