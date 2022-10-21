#ifndef ___FCOST_H___
#define ___FCOST_H___

#include <iostream>
#include <list>
#include <numeric>      // std::iota
#include <cmath>
//#include <algorithm>
#include <functional>
#include <memory>

struct CUSUM {
  double Sn = 0.0;
  int n = 0.0;
};


struct Piece {
  double St = 0.0;
  int tau = 0;
  double m0 = 0.0;
  double Mdiff = 0.0;

  // eval method has no generic, and it's distribution specific
  virtual double eval (const CUSUM& cs, double x, const double& theta0) const = 0;

  // this is the generic argmax method, that should work for all but gamma
  virtual double argmax (const CUSUM &cs ) const {
    return (cs.Sn - St) / (double)(cs.n - tau);
  }

};

struct PieceGau:Piece {
  double eval (const CUSUM& cs, double x, const double& theta0) const {
    auto c = (double)(cs.n - tau);
    auto S = (cs.Sn - St);

    // std::cout << "running gaussian" << std::endl;

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

    // std::cout << "running bernoulli" << std::endl;

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

// the cost is a list of shared pointers to pieces of type Piece
struct Cost {
  std::list<std::shared_ptr<Piece>> ps;
  double opt = 0;
};

struct Info {
  CUSUM cs;
  Cost Ql;
  Cost Qr;
  void update(const double& y, std::function<std::shared_ptr<Piece>(double, int, double)> newP, const double& thres, const double& theta0, const bool& adp_max_check);
};


// double argmax (const Piece &q, const CUSUM &cs );
// double argmax (const PieceGam &q, const CUSUM &cs );
//
// double eval (const PieceGau &q, const CUSUM &cs, double x, const double &theta0);
// double eval (const PieceBer &q, const CUSUM &cs, double x, const double &theta0);
// void prune (Cost &Q, const CUSUM &cs, const double &theta0, std::function<bool(Piece, Piece)> cond);
//Info focus_step (Info I, const double& y, std::function<std::shared_ptr<Piece>(double, int, double)> newP, const double& thres, const double& theta0, const bool& adp_max_check);

#endif
