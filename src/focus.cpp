# include "focus.h"

/*
 * ARGMAX FUNCTIONS NOW DEFINED IN METHODS!
 *
 * return the argmax of a given piece (needed for pruning and checkin the max)
 *
 */

// double argmax (const Piece &q, const CUSUM &cs ) {
//   return (cs.Sn - q.St) / (double)(cs.n - q.tau);
// }
//
//
// double argmax (const PieceGam &q, const CUSUM &cs ) {
//   return q.shape * (double)(cs.n - q.tau) / (cs.Sn - q.St);
// }


// missing exceptions for piece ber and piece poi


/*
 * PRUNING FUNCTION checks and removes quadratics that are no longer optimal
 */


void prune (Cost& Q, const CUSUM& cs, const double& theta0, const bool isRight) {

  // if we only have one or zero pieces, exit
  if (Q.ps.size() <= 1)
    return ;


  // this sets the condition
  std::function<bool(const std::unique_ptr<Piece>&, const std::unique_ptr<Piece>&)> cond;

  if (std::isnan(theta0)) {
    if (isRight) {
      cond = [cs, theta0](const std::unique_ptr<Piece>& q1, const std::unique_ptr<Piece>& q2)
      {
        return q1->argmax(cs) <= q2->argmax(cs);
      };
    } else {
      cond = [cs, theta0](const std::unique_ptr<Piece>& q1, const std::unique_ptr<Piece>& q2)
      {
        return q1->argmax(cs) >= q2->argmax(cs);
      };
    }
  } else {
    if (isRight) {
      cond = [cs, theta0](const std::unique_ptr<Piece>& q1, const std::unique_ptr<Piece>& q2)
      {
        return q1->argmax(cs) <= std::max(theta0, q2->argmax(cs));
      };
    } else {
      cond = [cs, theta0](const std::unique_ptr<Piece>& q1, const std::unique_ptr<Piece>& q2)
      {
        return q1->argmax(cs) >= std::min(theta0, q2->argmax(cs));
      };
    }

  }


  // reverse iterator at the end of the list
  auto qi = Q.ps.rbegin();


  // compare the last two quadratics
  while (cond((*qi), (*std::next(qi, 1)))) {

    // std::cout << "RUNNING!" << std::endl;

    // remove the last quadratic
    //(*qi).reset();
    Q.ps.pop_back();

    // std::cout << "PRUNED!" << std::endl;

    if (Q.ps.size() <= 1){
      break;
    }

    // reset the iterator to the last element
    qi = Q.ps.rbegin();


    // std::cout << "This should be fine..." << (*qi)->argmax(cs) << std::endl;
    // std::cout << "I think that here's the bug..." << (*std::next(qi, 1))->argmax(cs) << std::endl;


  }


}


/*
 * CHECK MAXIMA FUNCTIONS
 */

// get max of a single piece
//template <typename T>
double get_max (const std::unique_ptr<Piece>& q, const CUSUM& cs, const double& theta0) {
  //std::cout << q.eval(cs, argmax(q, cs), theta0) << std::endl;
  return q->eval(cs, q->argmax(cs), theta0);
}

double get_max_all (const Cost& Q, const CUSUM& cs, const double& theta0, const double& m0val) {

  auto max = 0.0;

  //std::cout << m0val << std::endl;

  for (auto& q:Q.ps) {


    max = std::max( max, get_max(q, cs, theta0) - m0val );
    //std::cout << "tau: " << q->tau << " st: " << q->St << " m0: " << q->m0 << " max-m0val: "<< max<< " | \n";
  }
  //std::cout << std::endl;

  return max;
}


// to write the adaptive maxima checking


/*
 * focus recursion, one iteration
 */

void Info::update(const double& y, std::function<std::unique_ptr<Piece>(double, int, double)> newP, const double& thres, const double& theta0, const bool& adp_max_check) {

  // updating the cusums and count with the new point
  cs.n ++;
  cs.Sn += y;

  //std::cout << "iteration: " << cs.n << std::endl;
  //std::cout << "cusum: " << cs.Sn << std::endl;


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
    //std::cout << "Qr maxs" << std::endl;

    Qr.opt = get_max_all(Qr, cs, theta0, m0val);
    //std::cout << "Ql maxs" << std::endl;

    Ql.opt = get_max_all(Ql, cs, theta0, m0val);
  }

  //std::cout << "Qr opt: " << Qr.opt << " Ql opt: " << Ql.opt << std::endl;

  // add a new point
  Qr.ps.push_back(std::move(newP(cs.Sn, cs.n, m0val)));
  Ql.ps.push_back(std::move(newP(cs.Sn, cs.n, m0val)));


  //std::cout << "__________________________" << std::endl;

}
