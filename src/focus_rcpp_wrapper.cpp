/*
 This code and all the code in this package is publicly released
 under the license GPL 3.0 available at https://cran.r-project.org/web/licenses/GPL-3
 See DESCRIPTION for further information about the authors.
 */

#include <Rcpp.h>
#include "focus.h"
using namespace Rcpp;

// [[Rcpp::export]]
List focus_offline (NumericVector Z, double threshold, String family, double theta0, List args, bool adp_max_check) {

  auto Y = clone(Z);

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
  } else if (family == "gamma") {
    auto shape_arg = args["shape"];
    newP = [shape_arg](double St, int tau, double m0){
      std::unique_ptr<Piece> p = std::make_unique<PieceGam>();
      p->St = St;
      p->tau = tau;
      p->m0 = m0;
      p->set_shape(shape_arg);
      return p;
    };
  } else if (family == "poisson") {
    newP = [](double St, int tau, double m0){
      std::unique_ptr<Piece> p = std::make_unique<PiecePoi>();
      p->St = St;
      p->tau = tau;
      p->m0 = m0;

      return p;
    };
  }

  // new init with constructor
  Info info(newP);

  // this is a temporary fix for the gaussian cost
  if (family == "gaussian" & !std::isnan(theta0)) {
    Y = Y - theta0;
    theta0 = 0;
  }

  std::list<double> stat;
  std::list<int> qlsize;
  std::list<int> qrsize;
  std::list<int> rk;
  std::list<int> lk;


  for (auto& y:Y) {
    info.update(y, newP, threshold, theta0, adp_max_check);
    stat.push_back(std::max(info.Ql.opt, info.Qr.opt));
    //
    qlsize.push_back(info.Ql.ps.size());
    qrsize.push_back(info.Qr.ps.size());
    rk.push_back(info.Qr.k);
    lk.push_back(info.Ql.k);


    if (stat.back() >= threshold)
      break;
  }

  return List::create(Rcpp::Named("stat") = stat,
                      Rcpp::Named("qrsize") = qrsize,
                      Rcpp::Named("qlsize") = qlsize,
                      Rcpp::Named("rk") = rk,
                      Rcpp::Named("lk") = lk,
                      Rcpp::Named("n") = info.cs.n);

}


// the following function will run NP-FOCuS in an offline setting.
// It will run Ber-FOCuS for all quantiles independently for 1(x < q) and record the result
// It will finally output a matrix of all the statistics for the independent quantiles, to be later analysed in R
// [[Rcpp::export(.npfocus_offline)]]
NumericMatrix npfocus_offline(NumericVector Y, const std::vector<double> quants, const std::vector<double> theta0, List args, bool adp_max_check) {

  auto pre_change_ukn = std::isnan(theta0[0]);

  NumericMatrix focus_stats(quants.size(), Y.size());

  std::function<std::unique_ptr<Piece>(double, int, double)> newP = [](double St, int tau, double m0){
    std::unique_ptr<Piece> p = std::make_unique<PieceBer>();
    p->St = St;
    p->tau = tau;
    p->m0 = m0;

    return p;
  };



  auto q_in = 0;
  for (auto q:quants) {
      // initializing a new info with bernoulli cost
    Info info(newP);
    auto y_in = 0;
    for (auto y:Y) {
      info.update((y <= q), newP, INFINITY, theta0[q_in], adp_max_check);
      focus_stats(q_in, y_in) = std::max(info.Ql.opt, info.Qr.opt);
      y_in++; // update counter for the observations
    }
    q_in++; // update counter for the quantiles
  }

  return focus_stats;

}


// You can include R code blocks in C++ files processed with sourceCpp
// (useful for testing and development). The R code will be automatically
// run after the compilation.
//

/*** R
theta0 <- 0
set.seed(45)
Y <- c(rnorm(1e3, theta0), rnorm(500, theta0 - 1))

system.time(res <- focus_offline(Y, 50, family = "gaussian", theta0 = NaN, args = list(), adp_max_check = F))

head(res$stat)
tail(res$stat)


#### bernoulli ####
theta0 <- 0.4
set.seed(45)
Y <- c(rbinom(1e3, 1, theta0), rbinom(1e3, 1, theta0 + .2))
system.time(res <- focus_offline(Y, 50, family = "bernoulli", theta0 = NaN, args = list(), adp_max_check = F))

head(res$stat, 5)
tail(res$stat)

plot(res$stat)


#### gamma ####
theta0 <- 4
shape <- 4
set.seed(42)
Y <- c(rgamma(500, rate = theta0, shape = shape), rgamma(500, rate = theta0 - 1, shape = shape))

system.time(res <- focus_offline(Y, 50, family = "gamma", theta0 = 1/theta0, args = list(shape = 4), adp_max_check = F))
plot(res$stat, type = "l")


### poisson ####
theta0 <- 2
set.seed(42)
Y <- c(rpois(500, theta0), rpois(500, 1.5))

system.time(res <- focus_offline(Y, 50, family = "poisson", theta0 = theta0, args = list(shape = 4), adp_max_check = F))
plot(res$stat, type = "l")

#### npfocus ####

theta0 <- 0
set.seed(45)
Y <- c(rnorm(1e3, theta0), rnorm(500, theta0 - 1))


*/
