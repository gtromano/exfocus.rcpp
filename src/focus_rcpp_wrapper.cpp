#include <Rcpp.h>
#include "focus.h"
using namespace Rcpp;


// [[Rcpp::export]]
NumericVector timesTwo(NumericVector x) {
  return x * 2;
}

// [[Rcpp::export]]
List focus_offline (NumericVector Y, double threshold, String family, double theta0, List args, bool adp_max_check) {

  // here we define the function to initialize a new piece
  std::function<std::unique_ptr<Piece>(double, int, double)> newP;

  if (family == "gaussian") {
    newP = [](double St, int tau, double m0){
      std::unique_ptr<Piece> p = std::make_unique<PieceGau>();
      p->St = St;
      p->tau = tau;
      p->m0 = m0;

      return p;
    };
  } else if (family == "bernoulli") {
    newP = [](double St, int tau, double m0){
      std::unique_ptr<Piece> p = std::make_unique<PieceBer>();
      p->St = St;
      p->tau = tau;
      p->m0 = m0;

      return p;
    };
  }

  //std::unique_ptr<Piece> test = newP(0.0, 0, 0.0);

  // initialization of the info vector. Default size  20.
  std::vector<std::unique_ptr<Piece>> initpsl;
  for (auto i = 0; i<50; i++) {
    initpsl.push_back(std::move(newP(0.0, 0, 0.0)));
  }


  std::vector<std::unique_ptr<Piece>> initpsr;
  for (auto i = 0; i<50; i++) {
    initpsr.push_back(std::move(newP(0.0, 0, 0.0)));
  }



  Cost initQl(std::move(initpsl), 0.0, 0);
  Cost initQr(std::move(initpsr), 0.0, 0);

  CUSUM initcusum;
  Info info(initcusum, std::move(initQl), std::move(initQr));

  // this is a temporary fix for the gaussian cost
  if (family == "gaussian" & !std::isnan(theta0)) {
    Y = Y - theta0;
    theta0 = 0;
  }

  // test
  //info.Ql.ps.front()->eval(info.cs, 1.0, 0.0);
  //info.Ql.ps.back()->eval(info.cs, 1.0, 0.0);


  std::list<double> stat;
  std::list<int> qlsize;
  std::list<int> qrsize;
  std::list<int> rk;
  std::list<int> lk;


  for (auto& y:Y) {
    info.update(y, newP, threshold, theta0, adp_max_check);
    // stat.push_back(std::max(info.Ql.opt, info.Qr.opt));
    //
    // qlsize.push_back(info.Ql.ps.size());
    // qrsize.push_back(info.Qr.ps.size());
    // rk.push_back(info.Qr.k);
    // lk.push_back(info.Ql.k);


    if (stat.back() >= threshold)
      break;
  }

  return List::create(Rcpp::Named("stat") = stat,
                      Rcpp::Named("qrsize") = qrsize,
                      Rcpp::Named("qlsize") = qlsize,
                      Rcpp::Named("rk") = rk,
                      Rcpp::Named("lk") = lk);

}



// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

/*** R
theta0 <- 0
set.seed(42)
Y <- c(rnorm(1e8, theta0), rnorm(500, theta0 - 1))

library(focus.new)
system.time(res <- focus_offline(Y, 50, family = "gaussian", theta0 = NaN, args = list(), adp_max_check = F))
#res$stat
#plot(res$stat, type = "l")

# library(FOCuS)
# system.time(res <- FOCuS(Y, 50))


*/
