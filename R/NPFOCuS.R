npfocus_offline <- function(Y, threshold, quantiles, theta0 = rep(NaN, length(quantiles))) {
    
    tmp <- .npfocus_offline(Y, quantiles, theta0, list(), adp_max_check = F)
    
    cppres <- tmp$focus_stat
    taures <- tmp$tau_stat

    sum_stat <- apply(cppres, 2, sum)
    max_stat <- apply(cppres, 2, max)
    
    n_sum <- which(sum_stat > threshold[1])[1]
    n_max <- which(max_stat > threshold[2])[1]
    
    n <- min(n_sum, n_max, na.rm = T)
    return(list(sum_stat = sum_stat,
                max_stat = max_stat,
                tau_stat = taures,
                n = n))
}


computeprobsNPPELT <- function (K, n = 100) {
  probs <- vector(length = K)
  c <- -log(2 * n - 1)
  for(i in 1:K) {
    probs[i] <- 1 / (1 +  (2 * n + 1) * exp( (c / K) * (2 * i - 1)) );
  }
  probs
}
