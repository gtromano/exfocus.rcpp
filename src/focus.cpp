/*
 This code and all the code in this package is publicly released
 under the license GPL 3.0 available at https://cran.r-project.org/web/licenses/GPL-3
 See DESCRIPTION for further information about the authors.
 */

# include "focus.h"


/*
 * PRUNING FUNCTION checks and removes quadratics that are no longer optimal
 */

void prune (Cost& Q, const CUSUM& cs, const double& theta0, const bool isRight) {

  // if we only have one piece, exit
  if (Q.k == 0)
    return ;


  // this sets the condition
  std::function<bool(const std::shared_ptr<Piece>&, const std::shared_ptr<Piece>&)> cond;

  if (std::isnan(theta0)) {
    if (isRight) {
      cond = [cs, theta0](const std::shared_ptr<Piece>& q1, const std::shared_ptr<Piece>& q2)
      {
        return q1->argmax(cs) <= q2->argmax(cs);
      };
    } else {
      cond = [cs, theta0](const std::shared_ptr<Piece>& q1, const std::shared_ptr<Piece>& q2)
      {
        return q1->argmax(cs) >= q2->argmax(cs);
      };
    }
  } else {
    if (isRight) {
      cond = [cs, theta0](const std::shared_ptr<Piece>& q1, const std::shared_ptr<Piece>& q2)
      {
        return q1->argmax(cs) <= std::max(theta0, q2->argmax(cs));
      };
    } else {
      cond = [cs, theta0](const std::shared_ptr<Piece>& q1, const std::shared_ptr<Piece>& q2)
      {
        return q1->argmax(cs) >= std::min(theta0, q2->argmax(cs));
      };
    }

  }


  // reverse iterator at the end of the list
  // auto qi = Q.ps.rbegin();


  // compare the last two quadratics
  while (cond(Q.ps[Q.k], Q.ps[Q.k - 1])) {

    // std::cout << "RUNNING!" << std::endl;

    // prune the last quadratic (we just drop the k value of one and we overwrite the other quadratics)
    Q.k--;

    // std::cout << "PRUNED!" << std::endl;

    if (Q.k == 0){
      break;
    }

    // std::cout << "This should be fine..." << (*qi)->argmax(cs) << std::endl;
    // std::cout << "I think that here's the bug..." << (*std::next(qi, 1))->argmax(cs) << std::endl;


  }


}


/*
 * CHECK MAXIMA FUNCTIONS
 */

// get max of a single piece
//template <typename T>
double get_max (const std::shared_ptr<Piece>& q, const CUSUM& cs, const double& theta0) {
  //std::cout << q.eval(cs, argmax(q, cs), theta0) << std::endl;
  return q->eval(cs, q->argmax(cs), theta0);
}

double get_max_all (const Cost& Q, const CUSUM& cs, const double& theta0, const double& m0val) {

  auto max = 0.0;

  for (auto i = 0; i <= Q.k; i++) {

    max = std::max( max, get_max(Q.ps[i], cs, theta0) - m0val );
    // std::cout << "tau: " << Q.ps[i]->tau << " st: " << Q.ps[i]->St << " m0: " << Q.ps[i]->m0 << " max-m0val: "<< max<< " | \n";
  }

  return max;
}


// to write the adaptive maxima checking


/*
 * focus recursion, one iteration
 */

void Info::update(const double& y, std::function<std::shared_ptr<Piece>(double, int, double)> newP, const double& thres, const double& theta0, const bool& adp_max_check) {

  // updating the cusums and count with the new point
  cs.n ++;
  cs.Sn += y;

  // updating the value of the max of the null (for pre-change mean unkown)
  auto m0val = 0.0;
  if (std::isnan(theta0))
    m0val = get_max(Qr.ps.front(), cs, theta0);

  // std::cout << "m0val: " << m0val << std::endl;

  // std::cout << "pruning step. size before pruning: " << Qr.ps.size() << std::endl;

  // pruning step
  prune(Qr, cs, theta0, true); // true for the right pruning
  prune(Ql, cs, theta0, false); // false for the left pruning

  // std::cout << "pruning step done. size after pruning: " << Qr.ps.size() << std::endl;


  // check the maximum
  if (adp_max_check) {
    // std::cout << "to write " << std::endl;
  } else {
    // std::cout << "Qr maxs" << std::endl;

    Qr.opt = get_max_all(Qr, cs, theta0, m0val);
    // std::cout << "Ql maxs" << std::endl;

    Ql.opt = get_max_all(Ql, cs, theta0, m0val);
  }

  // add a new point

  if (Qr.k < (Qr.ps.size() - 1)) {
    Qr.k ++; // incrase the counter of the last piece position
    Qr.ps[Qr.k]->St = cs.Sn;
    Qr.ps[Qr.k]->tau = cs.n;
    Qr.ps[Qr.k]->m0 = m0val;
  } else  {
    Qr.ps.push_back(std::move(newP(cs.Sn, cs.n, m0val)));
    Qr.k = Qr.ps.size() - 1; // incrase the counter of the last piece position
  }

  if (Ql.k < (Ql.ps.size() - 1)) {
    Ql.k ++; // incrase the counter of the last piece position
    Ql.ps[Ql.k]->St = cs.Sn;
    Ql.ps[Ql.k]->tau = cs.n;
    Ql.ps[Ql.k]->m0 = m0val;
  } else  {
    Ql.ps.push_back(std::move(newP(cs.Sn, cs.n, m0val)));
    Ql.k = Ql.ps.size() - 1; // incrase the counter of the last piece position

  }

}
