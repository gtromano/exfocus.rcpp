#include <Rcpp.h>
#include "focus.h"
using namespace Rcpp;

// Factory for Detector object for a given family
// [[Rcpp::export]]
SEXP detector_create(std::string family, double theta0, List args) {
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
    } else {
        stop("Unknown family");
    }

    Info* detector = new Info(newP, theta0);
    return XPtr<Info>(detector, true);
}

// [[Rcpp::export]]
void detector_update(SEXP detector_xptr, double y) {
    XPtr<Info> detector(detector_xptr);
    detector->update(y);
}

// [[Rcpp::export]]
double detector_statistic(SEXP detector_xptr) {
    XPtr<Info> detector(detector_xptr);
    return detector->statistic();
}