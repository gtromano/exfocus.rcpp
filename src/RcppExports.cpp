// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// focus_offline
List focus_offline(NumericVector Z, double threshold, String family, double theta0, List args, bool adp_max_check);
RcppExport SEXP _focus_new_focus_offline(SEXP ZSEXP, SEXP thresholdSEXP, SEXP familySEXP, SEXP theta0SEXP, SEXP argsSEXP, SEXP adp_max_checkSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericVector >::type Z(ZSEXP);
    Rcpp::traits::input_parameter< double >::type threshold(thresholdSEXP);
    Rcpp::traits::input_parameter< String >::type family(familySEXP);
    Rcpp::traits::input_parameter< double >::type theta0(theta0SEXP);
    Rcpp::traits::input_parameter< List >::type args(argsSEXP);
    Rcpp::traits::input_parameter< bool >::type adp_max_check(adp_max_checkSEXP);
    rcpp_result_gen = Rcpp::wrap(focus_offline(Z, threshold, family, theta0, args, adp_max_check));
    return rcpp_result_gen;
END_RCPP
}
// npfocus_offline
List npfocus_offline(NumericVector Y, const std::vector<double> quants, const std::vector<double> theta0, List args, bool adp_max_check);
RcppExport SEXP _focus_new_npfocus_offline(SEXP YSEXP, SEXP quantsSEXP, SEXP theta0SEXP, SEXP argsSEXP, SEXP adp_max_checkSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< NumericVector >::type Y(YSEXP);
    Rcpp::traits::input_parameter< const std::vector<double> >::type quants(quantsSEXP);
    Rcpp::traits::input_parameter< const std::vector<double> >::type theta0(theta0SEXP);
    Rcpp::traits::input_parameter< List >::type args(argsSEXP);
    Rcpp::traits::input_parameter< bool >::type adp_max_check(adp_max_checkSEXP);
    rcpp_result_gen = Rcpp::wrap(npfocus_offline(Y, quants, theta0, args, adp_max_check));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_focus_new_focus_offline", (DL_FUNC) &_focus_new_focus_offline, 6},
    {"_focus_new_npfocus_offline", (DL_FUNC) &_focus_new_npfocus_offline, 5},
    {NULL, NULL, 0}
};

RcppExport void R_init_focus_new(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
