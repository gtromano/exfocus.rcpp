

- [exfocus.rcpp](#exfocusrcpp)
  - [Installation](#installation)
  - [Available Functions](#available-functions)
  - [exfocus Rcpp offline implementation for testing
    purposes](#exfocus-rcpp-offline-implementation-for-testing-purposes)
  - [Gaussian](#gaussian)
    - [Pre change parameter known](#pre-change-parameter-known)
    - [pre-change unknown](#pre-change-unknown)
  - [Gamma](#gamma)
    - [known](#known)
    - [unknown](#unknown)
  - [Other distributions](#other-distributions)
- [exfocus online implementation](#exfocus-online-implementation)
- [NP-FOCuS](#np-focus)

# exfocus.rcpp

## Installation

You can install the development version of **exfocus.rcpp** from source
with:

``` r
# If you have devtools installed:
# devtools::install_github("yourusername/exfocus.rcpp")
# Or, if building locally:
# setwd("path/to/exfocus.rcpp")
# devtools::install()
```

Or, if you have the package source directory:

``` r
install.packages("path/to/exfocus.rcpp", repos = NULL, type = "source")
```

## Available Functions

- `focus_offline(...)`  
  Run the FOCuS detector in batch/offline mode (all cycles handled in
  C++).

- `npfocus_offline(...)`  
  Run the NP-FOCuS detector for multiple quantiles in batch/offline
  mode.

- `detector_create(family, theta0, args)`  
  Create a new online detector object for a given family.

- `detector_update(detector, y)`  
  Update the detector with a new observation.

- `detector_statistic(detector)`  
  Get the current value of the detector statistic.

- `computeprobsNPPELT(n)`  
  Compute quantile probabilities for NP-FOCuS.

## exfocus Rcpp offline implementation for testing purposes

The first set of examples demonstrates the **offline implementation**,
where all cycles and updates are handled internally in C++ for maximum
efficiency. This approach is ideal for benchmarking and testing, as it
closely reflects the performance you would expect from a pure C++
implementation.

While an **online implementation** is also available—allowing you to
update the detector sequentially from R—this will be inherently slower,
since each update involves a call from R into C++. The online interface
is useful for real-time or streaming scenarios, but is not as fast as
the fully internal C++ loop.

If you wish to use the library entirely in C++ (for maximum speed or
integration into other C++ projects), you can do so by following the
patterns in [`focus_rcpp_wrapper.cpp`](src/focus_rcpp_wrapper.cpp).

## Gaussian

### Pre change parameter known

Here we test the pre-change parameter known recursion, for gaussian
change in mean.

``` r
theta0 <- 0
set.seed(45)
Y <- c(rnorm(1e3, theta0), rnorm(500, theta0 - 1))

res <- focus_offline(Y, 50, family = "gaussian", theta0 = theta0, args = list(), adp_max_check = F)
plot(res$stat, type = "l")
```

![](generate_README_files/figure-commonmark/unnamed-chunk-2-1.png)

### pre-change unknown

To specify the pre-change unkown we have to set the theta0 parameter to
NaN.

``` r
theta0 <- 0
set.seed(45)
Y <- c(rnorm(1e3, theta0), rnorm(500, theta0 - 1))

res <- focus_offline(Y, 50, family = "gaussian", theta0 = NaN, args = list(), adp_max_check = F)
plot(res$stat, type = "l")
```

![](generate_README_files/figure-commonmark/unnamed-chunk-3-1.png)

## Gamma

### known

If you wish to perform exponential change in rate, set the shape
argument to 1!

``` r
theta0 <- 4
shape <- 4
set.seed(42)
Y <- c(rgamma(500, rate = theta0, shape = shape), rgamma(500, rate = theta0 - 1, shape = shape))

system.time(res <- focus_offline(Y, 50, family = "gamma", theta0 = 1/theta0, args = list(shape = 4), adp_max_check = F))
```

       user  system elapsed 
          0       0       0 

``` r
plot(res$stat, type = "l")
```

![](generate_README_files/figure-commonmark/unnamed-chunk-4-1.png)

### unknown

``` r
theta0 <- 4
shape <- 4
set.seed(42)
Y <- c(rgamma(500, rate = theta0, shape = shape), rgamma(500, rate = theta0 - 1, shape = shape))

# note that I have coded it for change in scale! As in the paper, but this can be changed any time in the interface. 
system.time(res <- focus_offline(Y, 50, family = "gamma", theta0 = NaN, args = list(shape = 4), adp_max_check = F))
```

       user  system elapsed 
          0       0       0 

``` r
plot(res$stat, type = "l")
```

![](generate_README_files/figure-commonmark/unnamed-chunk-5-1.png)

## Other distributions

Similarly, we have Bernoulli and Poisson.

``` r
### poisson ####
theta0 <- 2
set.seed(42)
Y <- c(rpois(500, theta0), rpois(500, 1.5))

system.time(res <- focus_offline(Y, 50, family = "poisson", theta0 = theta0, args = list(shape = 4), adp_max_check = F))
```

       user  system elapsed 
          0       0       0 

``` r
plot(res$stat, type = "l")
```

![](generate_README_files/figure-commonmark/unnamed-chunk-6-1.png)

``` r
#### bernoulli ####
theta0 <- 0.4
set.seed(45)
Y <- c(rbinom(1e3, 1, theta0), rbinom(1e3, 1, theta0 + .2))
system.time(res <- focus_offline(Y, 50, family = "bernoulli", theta0 = theta0, args = list(), adp_max_check = F))
```

       user  system elapsed 
      0.000   0.000   0.001 

``` r
plot(res$stat, type = "l")
```

![](generate_README_files/figure-commonmark/unnamed-chunk-6-2.png)

# exfocus online implementation

It is possible to run the FOCuS detector in an online (sequential)
fashion, updating the statistic as new data arrives. This is useful for
real-time change detection, where you want to process each observation
as it comes in, rather than in batch.

``` r
theta0 <- 0
set.seed(45)
Y <- c(rnorm(1e3, theta0), rnorm(500, theta0 - 1))

# Create a detector for the Gaussian family with unknown pre-change mean
det <- detector_create("gaussian", NaN, list())

# Sequentially update the detector and record the statistic
stat_trace <- numeric(length(Y))
for (i in seq_along(Y)) {
  detector_update(det, Y[i])
  stat_trace[i] <- detector_statistic(det)
}

plot(stat_trace, type = "l", main = "Online FOCuS Statistic (Gaussian)", ylab = "Statistic", xlab = "Time")
```

![](generate_README_files/figure-commonmark/unnamed-chunk-7-1.png)

# NP-FOCuS

Finally, NP-FOCuS can run as:

``` r
set.seed(42)
Y <- c(rnorm(5000), rnorm(100, sd = 2))


probs <- computeprobsNPPELT(15)
quants <- qnorm(probs)
system.time(res <- npfocus_offline(Y = Y, threshold = c(90, 15), quantiles = quants))
```

       user  system elapsed 
      0.029   0.000   0.029 

``` r
par(mfrow = c(2, 1))
plot(res$max_stat, type = "l")
plot(res$sum_stat, type = "l")
```

![](generate_README_files/figure-commonmark/unnamed-chunk-8-1.png)

``` r
par(mfrow = c(1, 1))

which.max(table(apply(res$tau_stat[, 1:res$n], 2, median)))
```

    647 
    116 
