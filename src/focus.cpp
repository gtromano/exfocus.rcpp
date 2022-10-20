// # include "focus.h"
//
// /*
//  * ARGMAX FUNCTIONS NOW DEFINED IN METHODS!
//  *
//  * return the argmax of a given piece (needed for pruning and checkin the max)
//  *
//  */
//
// // double argmax (const Piece &q, const CUSUM &cs ) {
// //   return (cs.Sn - q.St) / (double)(cs.n - q.tau);
// // }
// //
// //
// // double argmax (const PieceGam &q, const CUSUM &cs ) {
// //   return q.shape * (double)(cs.n - q.tau) / (cs.Sn - q.St);
// // }
//
//
// // missing exceptions for piece ber and piece poi
//
//
// /*
//  * PRUNING FUNCTION checks and removes quadratics that are no longer optimal
//  */
//
//
// void prune (Cost& Q, const CUSUM& cs, const double& theta0, const bool isRight) {
//
//   // if we only have one or zero pieces, exit
//   if (Q.ps.size() <= 1)
//     return ;
//
//
//   // this sets the condition
//   std::function<bool(Piece, Piece)> cond;
//
//   if (std::isnan(theta0)) {
//     if (isRight) {
//       cond = [cs, theta0](Piece q1, Piece q2)
//       {
//         return argmax(q1, cs) <= argmax(q2, cs);
//       };
//     } else {
//       cond = [cs, theta0](Piece q1, Piece q2)
//       {
//         return argmax(q1, cs) >= argmax(q2, cs);
//       };
//     }
//   } else {
//     if (isRight) {
//       cond = [cs, theta0](Piece q1, Piece q2)
//       {
//         return argmax(q1, cs) <= std::max(theta0, argmax(q2, cs));
//       };
//     } else {
//       cond = [cs, theta0](Piece q1, Piece q2)
//       {
//         return argmax(q1, cs) >= std::min(theta0, argmax(q2, cs));
//       };
//     }
//
//   }
//
//
//   // reverse iterator at the end of the list
//   auto qi = Q.ps.rbegin();
//
//
//   // compare the last two quadratics
//   while (cond((*qi), (*std::next(qi, 1))) & Q.ps.size() > 1) {
//
//     //std::cout << "RUNNING!" << std::endl;
//
//
//     // remove the last quadratic
//     Q.ps.pop_back();
//
//     // reset the iterator to the last element
//     qi = Q.ps.rbegin();
//
//   }
//
// }
//
//
// /*
//  * CHECK MAXIMA FUNCTIONS
//  */
//
// // get max of a single piece
// //template <typename T>
// double get_max (const auto& q, const CUSUM& cs, const double& theta0) {
//   //std::cout << q.eval(cs, argmax(q, cs), theta0) << std::endl;
//   return q.eval(cs, argmax(q, cs), theta0);
// }
//
// template <typename T>
// double get_max_all (const Cost<auto> &Q, const CUSUM &cs, const double &theta0, const double &m0val) {
//
//   auto max = 0.0;
//
//   for (auto&q:Q.ps) {
//     max = std::max( 0.0, get_max(q, cs, theta0) - m0val );
//
//   }
//
//   return max;
// }
//
//
// // to write the adaptive maxima checking
//
//
// /*
//  * focus recursion, one iteration
//  */
//
// // here info is used by copy, not reference, to try with std::move later
// auto focus_step (Info<auto> I, const double& y, auto newP, const double& thres, const double& theta0, const bool& adp_max_check) {
//
//   // updating the cusums and count with the new point
//   I.cs.n ++;
//   I.cs.Sn += y;
//
//   std::cout << "iteration: " << I.cs.n << std::endl;
//   std::cout << "cusum: " << I.cs.Sn << std::endl;
//
//
//   // updating the value of the max of the null (for pre-change mean unkown)
//   auto m0val = 0.0;
//   // if (std::isnan(theta0))
//   //   m0val = get_max(newP(0.0, 0, 0.0), I.cs, theta0);
//
//   if (std::isnan(theta0)) {
//     m0val = get_max(newP, I.cs, theta0);
//
//   }
//
//   std::cout << "m0val: " << m0val << std::endl;
//
//   std::cout << "pruning step. size before pruning: " << I.Qr.ps.size() << std::endl;
//
//   // pruning step
//   prune(I.Qr, I.cs, theta0, true); // true for the right pruning
//   prune(I.Ql, I.cs, theta0, false); // false for the left pruning
//
//   std::cout << "pruning step done. size after pruning: " << I.Qr.ps.size() << std::endl;
//
//
//   // check the maximum
//   if (adp_max_check) {
//     std::cout << "to write " << std::endl;
//   } else {
//
//     I.Qr.opt = get_max_all(I.Qr, I.cs, theta0, m0val);
//     I.Ql.opt = get_max_all(I.Ql, I.cs, theta0, m0val);
//   }
//
//   // add a new point
//   //I.Qr.ps.push_back(newP(I.cs.Sn, I.cs.n, m0val));
//   //I.Ql.ps.push_back(newP(I.cs.Sn, I.cs.n, m0val));
//
//   newP.St = I.cs.Sn;
//   newP.tau =  I.cs.n;
//   newP.m0 = m0val;
//
//   I.Qr.ps.push_back(newP);
//   I.Ql.ps.push_back(newP);
//
//
//   std::cout << "__________________________" << std::endl;
//
//   return I;
// }
