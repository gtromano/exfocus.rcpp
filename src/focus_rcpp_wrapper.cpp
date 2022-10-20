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
  std::function<std::shared_ptr<Piece>(double, int, double)> newP;

  if (family == "gaussian") {
    newP = [](double St, int tau, double m0){
      std::shared_ptr<Piece> p = std::make_shared<PieceGau>();
      p->St = St;
      p->tau = tau;
      p->m0 = m0;

      return p;
    };
  } else if (family == "bernoulli") {
    newP = [](double St, int tau, double m0){
      std::shared_ptr<Piece> p = std::make_shared<PieceBer>();
      p->St = St;
      p->tau = tau;
      p->m0 = m0;

      return p;
    };
  }


  // initialization of the info list
  Cost initcost({newP(0.0, 0, 0.0)}, 0.0);
  CUSUM initcusum;
  Info info(initcusum, initcost, initcost);

  // this is a temporary fix for the gaussian cost
  if (family == "gaussian" & !std::isnan(theta0)) {
    Y = Y - theta0;
    theta0 = 0;
  }

  // test
  info.Ql.ps.front()->eval(info.cs, 1.0, 0.0);

  std::list<double> stat;
  std::list<int> qlsize;
  std::list<int> qrsize;

  for (auto& y:Y) {
    //info = focus_step(info, y, newP, threshold, theta0, adp_max_check);
    stat.push_back(std::max(info.Ql.opt, info.Qr.opt));

    qlsize.push_back(info.Ql.ps.size());
    qrsize.push_back(info.Qr.ps.size());

    if (stat.back() >= threshold)
      break;
  }

  return List::create(Rcpp::Named("stat") = stat,
                      Rcpp::Named("qrsize") = qrsize,
                      Rcpp::Named("qlsize") = qlsize);

}



// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

/*** R
theta0 <- 0
set.seed(42)
Y <- c(rnorm(5, theta0), rnorm(2, theta0 - 1))

res <- focus_offline(Y, 17, family = "bernoulli", theta0 = NaN, args = list(), adp_max_check = F)
plot(res$stat, type = "l")
*/
