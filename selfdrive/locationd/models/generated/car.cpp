#include "car.h"

namespace {
#define DIM 9
#define EDIM 9
#define MEDIM 9
typedef void (*Hfun)(double *, double *, double *);

double mass;

void set_mass(double x){ mass = x;}

double rotational_inertia;

void set_rotational_inertia(double x){ rotational_inertia = x;}

double center_to_front;

void set_center_to_front(double x){ center_to_front = x;}

double center_to_rear;

void set_center_to_rear(double x){ center_to_rear = x;}

double stiffness_front;

void set_stiffness_front(double x){ stiffness_front = x;}

double stiffness_rear;

void set_stiffness_rear(double x){ stiffness_rear = x;}
const static double MAHA_THRESH_25 = 3.8414588206941227;
const static double MAHA_THRESH_24 = 5.991464547107981;
const static double MAHA_THRESH_30 = 3.8414588206941227;
const static double MAHA_THRESH_26 = 3.8414588206941227;
const static double MAHA_THRESH_27 = 3.8414588206941227;
const static double MAHA_THRESH_29 = 3.8414588206941227;
const static double MAHA_THRESH_28 = 3.8414588206941227;
const static double MAHA_THRESH_31 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.11.1                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_2123858726471964323) {
   out_2123858726471964323[0] = delta_x[0] + nom_x[0];
   out_2123858726471964323[1] = delta_x[1] + nom_x[1];
   out_2123858726471964323[2] = delta_x[2] + nom_x[2];
   out_2123858726471964323[3] = delta_x[3] + nom_x[3];
   out_2123858726471964323[4] = delta_x[4] + nom_x[4];
   out_2123858726471964323[5] = delta_x[5] + nom_x[5];
   out_2123858726471964323[6] = delta_x[6] + nom_x[6];
   out_2123858726471964323[7] = delta_x[7] + nom_x[7];
   out_2123858726471964323[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_147094582364114116) {
   out_147094582364114116[0] = -nom_x[0] + true_x[0];
   out_147094582364114116[1] = -nom_x[1] + true_x[1];
   out_147094582364114116[2] = -nom_x[2] + true_x[2];
   out_147094582364114116[3] = -nom_x[3] + true_x[3];
   out_147094582364114116[4] = -nom_x[4] + true_x[4];
   out_147094582364114116[5] = -nom_x[5] + true_x[5];
   out_147094582364114116[6] = -nom_x[6] + true_x[6];
   out_147094582364114116[7] = -nom_x[7] + true_x[7];
   out_147094582364114116[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_4046455293741089611) {
   out_4046455293741089611[0] = 1.0;
   out_4046455293741089611[1] = 0;
   out_4046455293741089611[2] = 0;
   out_4046455293741089611[3] = 0;
   out_4046455293741089611[4] = 0;
   out_4046455293741089611[5] = 0;
   out_4046455293741089611[6] = 0;
   out_4046455293741089611[7] = 0;
   out_4046455293741089611[8] = 0;
   out_4046455293741089611[9] = 0;
   out_4046455293741089611[10] = 1.0;
   out_4046455293741089611[11] = 0;
   out_4046455293741089611[12] = 0;
   out_4046455293741089611[13] = 0;
   out_4046455293741089611[14] = 0;
   out_4046455293741089611[15] = 0;
   out_4046455293741089611[16] = 0;
   out_4046455293741089611[17] = 0;
   out_4046455293741089611[18] = 0;
   out_4046455293741089611[19] = 0;
   out_4046455293741089611[20] = 1.0;
   out_4046455293741089611[21] = 0;
   out_4046455293741089611[22] = 0;
   out_4046455293741089611[23] = 0;
   out_4046455293741089611[24] = 0;
   out_4046455293741089611[25] = 0;
   out_4046455293741089611[26] = 0;
   out_4046455293741089611[27] = 0;
   out_4046455293741089611[28] = 0;
   out_4046455293741089611[29] = 0;
   out_4046455293741089611[30] = 1.0;
   out_4046455293741089611[31] = 0;
   out_4046455293741089611[32] = 0;
   out_4046455293741089611[33] = 0;
   out_4046455293741089611[34] = 0;
   out_4046455293741089611[35] = 0;
   out_4046455293741089611[36] = 0;
   out_4046455293741089611[37] = 0;
   out_4046455293741089611[38] = 0;
   out_4046455293741089611[39] = 0;
   out_4046455293741089611[40] = 1.0;
   out_4046455293741089611[41] = 0;
   out_4046455293741089611[42] = 0;
   out_4046455293741089611[43] = 0;
   out_4046455293741089611[44] = 0;
   out_4046455293741089611[45] = 0;
   out_4046455293741089611[46] = 0;
   out_4046455293741089611[47] = 0;
   out_4046455293741089611[48] = 0;
   out_4046455293741089611[49] = 0;
   out_4046455293741089611[50] = 1.0;
   out_4046455293741089611[51] = 0;
   out_4046455293741089611[52] = 0;
   out_4046455293741089611[53] = 0;
   out_4046455293741089611[54] = 0;
   out_4046455293741089611[55] = 0;
   out_4046455293741089611[56] = 0;
   out_4046455293741089611[57] = 0;
   out_4046455293741089611[58] = 0;
   out_4046455293741089611[59] = 0;
   out_4046455293741089611[60] = 1.0;
   out_4046455293741089611[61] = 0;
   out_4046455293741089611[62] = 0;
   out_4046455293741089611[63] = 0;
   out_4046455293741089611[64] = 0;
   out_4046455293741089611[65] = 0;
   out_4046455293741089611[66] = 0;
   out_4046455293741089611[67] = 0;
   out_4046455293741089611[68] = 0;
   out_4046455293741089611[69] = 0;
   out_4046455293741089611[70] = 1.0;
   out_4046455293741089611[71] = 0;
   out_4046455293741089611[72] = 0;
   out_4046455293741089611[73] = 0;
   out_4046455293741089611[74] = 0;
   out_4046455293741089611[75] = 0;
   out_4046455293741089611[76] = 0;
   out_4046455293741089611[77] = 0;
   out_4046455293741089611[78] = 0;
   out_4046455293741089611[79] = 0;
   out_4046455293741089611[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_2561792606746991469) {
   out_2561792606746991469[0] = state[0];
   out_2561792606746991469[1] = state[1];
   out_2561792606746991469[2] = state[2];
   out_2561792606746991469[3] = state[3];
   out_2561792606746991469[4] = state[4];
   out_2561792606746991469[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_2561792606746991469[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_2561792606746991469[7] = state[7];
   out_2561792606746991469[8] = state[8];
}
void F_fun(double *state, double dt, double *out_6749487635925460135) {
   out_6749487635925460135[0] = 1;
   out_6749487635925460135[1] = 0;
   out_6749487635925460135[2] = 0;
   out_6749487635925460135[3] = 0;
   out_6749487635925460135[4] = 0;
   out_6749487635925460135[5] = 0;
   out_6749487635925460135[6] = 0;
   out_6749487635925460135[7] = 0;
   out_6749487635925460135[8] = 0;
   out_6749487635925460135[9] = 0;
   out_6749487635925460135[10] = 1;
   out_6749487635925460135[11] = 0;
   out_6749487635925460135[12] = 0;
   out_6749487635925460135[13] = 0;
   out_6749487635925460135[14] = 0;
   out_6749487635925460135[15] = 0;
   out_6749487635925460135[16] = 0;
   out_6749487635925460135[17] = 0;
   out_6749487635925460135[18] = 0;
   out_6749487635925460135[19] = 0;
   out_6749487635925460135[20] = 1;
   out_6749487635925460135[21] = 0;
   out_6749487635925460135[22] = 0;
   out_6749487635925460135[23] = 0;
   out_6749487635925460135[24] = 0;
   out_6749487635925460135[25] = 0;
   out_6749487635925460135[26] = 0;
   out_6749487635925460135[27] = 0;
   out_6749487635925460135[28] = 0;
   out_6749487635925460135[29] = 0;
   out_6749487635925460135[30] = 1;
   out_6749487635925460135[31] = 0;
   out_6749487635925460135[32] = 0;
   out_6749487635925460135[33] = 0;
   out_6749487635925460135[34] = 0;
   out_6749487635925460135[35] = 0;
   out_6749487635925460135[36] = 0;
   out_6749487635925460135[37] = 0;
   out_6749487635925460135[38] = 0;
   out_6749487635925460135[39] = 0;
   out_6749487635925460135[40] = 1;
   out_6749487635925460135[41] = 0;
   out_6749487635925460135[42] = 0;
   out_6749487635925460135[43] = 0;
   out_6749487635925460135[44] = 0;
   out_6749487635925460135[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_6749487635925460135[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_6749487635925460135[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_6749487635925460135[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_6749487635925460135[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_6749487635925460135[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_6749487635925460135[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_6749487635925460135[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_6749487635925460135[53] = -9.8000000000000007*dt;
   out_6749487635925460135[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_6749487635925460135[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_6749487635925460135[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_6749487635925460135[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_6749487635925460135[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_6749487635925460135[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_6749487635925460135[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_6749487635925460135[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_6749487635925460135[62] = 0;
   out_6749487635925460135[63] = 0;
   out_6749487635925460135[64] = 0;
   out_6749487635925460135[65] = 0;
   out_6749487635925460135[66] = 0;
   out_6749487635925460135[67] = 0;
   out_6749487635925460135[68] = 0;
   out_6749487635925460135[69] = 0;
   out_6749487635925460135[70] = 1;
   out_6749487635925460135[71] = 0;
   out_6749487635925460135[72] = 0;
   out_6749487635925460135[73] = 0;
   out_6749487635925460135[74] = 0;
   out_6749487635925460135[75] = 0;
   out_6749487635925460135[76] = 0;
   out_6749487635925460135[77] = 0;
   out_6749487635925460135[78] = 0;
   out_6749487635925460135[79] = 0;
   out_6749487635925460135[80] = 1;
}
void h_25(double *state, double *unused, double *out_4036485789383440967) {
   out_4036485789383440967[0] = state[6];
}
void H_25(double *state, double *unused, double *out_5024108742981173493) {
   out_5024108742981173493[0] = 0;
   out_5024108742981173493[1] = 0;
   out_5024108742981173493[2] = 0;
   out_5024108742981173493[3] = 0;
   out_5024108742981173493[4] = 0;
   out_5024108742981173493[5] = 0;
   out_5024108742981173493[6] = 1;
   out_5024108742981173493[7] = 0;
   out_5024108742981173493[8] = 0;
}
void h_24(double *state, double *unused, double *out_2369282182506852092) {
   out_2369282182506852092[0] = state[4];
   out_2369282182506852092[1] = state[5];
}
void H_24(double *state, double *unused, double *out_4194570144659182898) {
   out_4194570144659182898[0] = 0;
   out_4194570144659182898[1] = 0;
   out_4194570144659182898[2] = 0;
   out_4194570144659182898[3] = 0;
   out_4194570144659182898[4] = 1;
   out_4194570144659182898[5] = 0;
   out_4194570144659182898[6] = 0;
   out_4194570144659182898[7] = 0;
   out_4194570144659182898[8] = 0;
   out_4194570144659182898[9] = 0;
   out_4194570144659182898[10] = 0;
   out_4194570144659182898[11] = 0;
   out_4194570144659182898[12] = 0;
   out_4194570144659182898[13] = 0;
   out_4194570144659182898[14] = 1;
   out_4194570144659182898[15] = 0;
   out_4194570144659182898[16] = 0;
   out_4194570144659182898[17] = 0;
}
void h_30(double *state, double *unused, double *out_5272540999642259467) {
   out_5272540999642259467[0] = state[4];
}
void H_30(double *state, double *unused, double *out_496412412853565295) {
   out_496412412853565295[0] = 0;
   out_496412412853565295[1] = 0;
   out_496412412853565295[2] = 0;
   out_496412412853565295[3] = 0;
   out_496412412853565295[4] = 1;
   out_496412412853565295[5] = 0;
   out_496412412853565295[6] = 0;
   out_496412412853565295[7] = 0;
   out_496412412853565295[8] = 0;
}
void h_26(double *state, double *unused, double *out_8436057024560478219) {
   out_8436057024560478219[0] = state[7];
}
void H_26(double *state, double *unused, double *out_1282605424107117269) {
   out_1282605424107117269[0] = 0;
   out_1282605424107117269[1] = 0;
   out_1282605424107117269[2] = 0;
   out_1282605424107117269[3] = 0;
   out_1282605424107117269[4] = 0;
   out_1282605424107117269[5] = 0;
   out_1282605424107117269[6] = 0;
   out_1282605424107117269[7] = 1;
   out_1282605424107117269[8] = 0;
}
void h_27(double *state, double *unused, double *out_2005415766435312201) {
   out_2005415766435312201[0] = state[3];
}
void H_27(double *state, double *unused, double *out_2720006484037508512) {
   out_2720006484037508512[0] = 0;
   out_2720006484037508512[1] = 0;
   out_2720006484037508512[2] = 0;
   out_2720006484037508512[3] = 1;
   out_2720006484037508512[4] = 0;
   out_2720006484037508512[5] = 0;
   out_2720006484037508512[6] = 0;
   out_2720006484037508512[7] = 0;
   out_2720006484037508512[8] = 0;
}
void h_29(double *state, double *unused, double *out_7531473202986335314) {
   out_7531473202986335314[0] = state[1];
}
void H_29(double *state, double *unused, double *out_1006643757167957479) {
   out_1006643757167957479[0] = 0;
   out_1006643757167957479[1] = 1;
   out_1006643757167957479[2] = 0;
   out_1006643757167957479[3] = 0;
   out_1006643757167957479[4] = 0;
   out_1006643757167957479[5] = 0;
   out_1006643757167957479[6] = 0;
   out_1006643757167957479[7] = 0;
   out_1006643757167957479[8] = 0;
}
void h_28(double *state, double *unused, double *out_8035671487630012762) {
   out_8035671487630012762[0] = state[0];
}
void H_28(double *state, double *unused, double *out_4075755259901573095) {
   out_4075755259901573095[0] = 1;
   out_4075755259901573095[1] = 0;
   out_4075755259901573095[2] = 0;
   out_4075755259901573095[3] = 0;
   out_4075755259901573095[4] = 0;
   out_4075755259901573095[5] = 0;
   out_4075755259901573095[6] = 0;
   out_4075755259901573095[7] = 0;
   out_4075755259901573095[8] = 0;
}
void h_31(double *state, double *unused, double *out_5556457641467274679) {
   out_5556457641467274679[0] = state[8];
}
void H_31(double *state, double *unused, double *out_656397321873765793) {
   out_656397321873765793[0] = 0;
   out_656397321873765793[1] = 0;
   out_656397321873765793[2] = 0;
   out_656397321873765793[3] = 0;
   out_656397321873765793[4] = 0;
   out_656397321873765793[5] = 0;
   out_656397321873765793[6] = 0;
   out_656397321873765793[7] = 0;
   out_656397321873765793[8] = 1;
}
#include <eigen3/Eigen/Dense>
#include <iostream>

typedef Eigen::Matrix<double, DIM, DIM, Eigen::RowMajor> DDM;
typedef Eigen::Matrix<double, EDIM, EDIM, Eigen::RowMajor> EEM;
typedef Eigen::Matrix<double, DIM, EDIM, Eigen::RowMajor> DEM;

void predict(double *in_x, double *in_P, double *in_Q, double dt) {
  typedef Eigen::Matrix<double, MEDIM, MEDIM, Eigen::RowMajor> RRM;

  double nx[DIM] = {0};
  double in_F[EDIM*EDIM] = {0};

  // functions from sympy
  f_fun(in_x, dt, nx);
  F_fun(in_x, dt, in_F);


  EEM F(in_F);
  EEM P(in_P);
  EEM Q(in_Q);

  RRM F_main = F.topLeftCorner(MEDIM, MEDIM);
  P.topLeftCorner(MEDIM, MEDIM) = (F_main * P.topLeftCorner(MEDIM, MEDIM)) * F_main.transpose();
  P.topRightCorner(MEDIM, EDIM - MEDIM) = F_main * P.topRightCorner(MEDIM, EDIM - MEDIM);
  P.bottomLeftCorner(EDIM - MEDIM, MEDIM) = P.bottomLeftCorner(EDIM - MEDIM, MEDIM) * F_main.transpose();

  P = P + dt*Q;

  // copy out state
  memcpy(in_x, nx, DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
}

// note: extra_args dim only correct when null space projecting
// otherwise 1
template <int ZDIM, int EADIM, bool MAHA_TEST>
void update(double *in_x, double *in_P, Hfun h_fun, Hfun H_fun, Hfun Hea_fun, double *in_z, double *in_R, double *in_ea, double MAHA_THRESHOLD) {
  typedef Eigen::Matrix<double, ZDIM, ZDIM, Eigen::RowMajor> ZZM;
  typedef Eigen::Matrix<double, ZDIM, DIM, Eigen::RowMajor> ZDM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, EDIM, Eigen::RowMajor> XEM;
  //typedef Eigen::Matrix<double, EDIM, ZDIM, Eigen::RowMajor> EZM;
  typedef Eigen::Matrix<double, Eigen::Dynamic, 1> X1M;
  typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> XXM;

  double in_hx[ZDIM] = {0};
  double in_H[ZDIM * DIM] = {0};
  double in_H_mod[EDIM * DIM] = {0};
  double delta_x[EDIM] = {0};
  double x_new[DIM] = {0};


  // state x, P
  Eigen::Matrix<double, ZDIM, 1> z(in_z);
  EEM P(in_P);
  ZZM pre_R(in_R);

  // functions from sympy
  h_fun(in_x, in_ea, in_hx);
  H_fun(in_x, in_ea, in_H);
  ZDM pre_H(in_H);

  // get y (y = z - hx)
  Eigen::Matrix<double, ZDIM, 1> pre_y(in_hx); pre_y = z - pre_y;
  X1M y; XXM H; XXM R;
  if (Hea_fun){
    typedef Eigen::Matrix<double, ZDIM, EADIM, Eigen::RowMajor> ZAM;
    double in_Hea[ZDIM * EADIM] = {0};
    Hea_fun(in_x, in_ea, in_Hea);
    ZAM Hea(in_Hea);
    XXM A = Hea.transpose().fullPivLu().kernel();


    y = A.transpose() * pre_y;
    H = A.transpose() * pre_H;
    R = A.transpose() * pre_R * A;
  } else {
    y = pre_y;
    H = pre_H;
    R = pre_R;
  }
  // get modified H
  H_mod_fun(in_x, in_H_mod);
  DEM H_mod(in_H_mod);
  XEM H_err = H * H_mod;

  // Do mahalobis distance test
  if (MAHA_TEST){
    XXM a = (H_err * P * H_err.transpose() + R).inverse();
    double maha_dist = y.transpose() * a * y;
    if (maha_dist > MAHA_THRESHOLD){
      R = 1.0e16 * R;
    }
  }

  // Outlier resilient weighting
  double weight = 1;//(1.5)/(1 + y.squaredNorm()/R.sum());

  // kalman gains and I_KH
  XXM S = ((H_err * P) * H_err.transpose()) + R/weight;
  XEM KT = S.fullPivLu().solve(H_err * P.transpose());
  //EZM K = KT.transpose(); TODO: WHY DOES THIS NOT COMPILE?
  //EZM K = S.fullPivLu().solve(H_err * P.transpose()).transpose();
  //std::cout << "Here is the matrix rot:\n" << K << std::endl;
  EEM I_KH = Eigen::Matrix<double, EDIM, EDIM>::Identity() - (KT.transpose() * H_err);

  // update state by injecting dx
  Eigen::Matrix<double, EDIM, 1> dx(delta_x);
  dx  = (KT.transpose() * y);
  memcpy(delta_x, dx.data(), EDIM * sizeof(double));
  err_fun(in_x, delta_x, x_new);
  Eigen::Matrix<double, DIM, 1> x(x_new);

  // update cov
  P = ((I_KH * P) * I_KH.transpose()) + ((KT.transpose() * R) * KT);

  // copy out state
  memcpy(in_x, x.data(), DIM * sizeof(double));
  memcpy(in_P, P.data(), EDIM * EDIM * sizeof(double));
  memcpy(in_z, y.data(), y.rows() * sizeof(double));
}




}
extern "C" {

void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_25, H_25, NULL, in_z, in_R, in_ea, MAHA_THRESH_25);
}
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<2, 3, 0>(in_x, in_P, h_24, H_24, NULL, in_z, in_R, in_ea, MAHA_THRESH_24);
}
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_30, H_30, NULL, in_z, in_R, in_ea, MAHA_THRESH_30);
}
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_26, H_26, NULL, in_z, in_R, in_ea, MAHA_THRESH_26);
}
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_27, H_27, NULL, in_z, in_R, in_ea, MAHA_THRESH_27);
}
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_29, H_29, NULL, in_z, in_R, in_ea, MAHA_THRESH_29);
}
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_28, H_28, NULL, in_z, in_R, in_ea, MAHA_THRESH_28);
}
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_31, H_31, NULL, in_z, in_R, in_ea, MAHA_THRESH_31);
}
void car_err_fun(double *nom_x, double *delta_x, double *out_2123858726471964323) {
  err_fun(nom_x, delta_x, out_2123858726471964323);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_147094582364114116) {
  inv_err_fun(nom_x, true_x, out_147094582364114116);
}
void car_H_mod_fun(double *state, double *out_4046455293741089611) {
  H_mod_fun(state, out_4046455293741089611);
}
void car_f_fun(double *state, double dt, double *out_2561792606746991469) {
  f_fun(state,  dt, out_2561792606746991469);
}
void car_F_fun(double *state, double dt, double *out_6749487635925460135) {
  F_fun(state,  dt, out_6749487635925460135);
}
void car_h_25(double *state, double *unused, double *out_4036485789383440967) {
  h_25(state, unused, out_4036485789383440967);
}
void car_H_25(double *state, double *unused, double *out_5024108742981173493) {
  H_25(state, unused, out_5024108742981173493);
}
void car_h_24(double *state, double *unused, double *out_2369282182506852092) {
  h_24(state, unused, out_2369282182506852092);
}
void car_H_24(double *state, double *unused, double *out_4194570144659182898) {
  H_24(state, unused, out_4194570144659182898);
}
void car_h_30(double *state, double *unused, double *out_5272540999642259467) {
  h_30(state, unused, out_5272540999642259467);
}
void car_H_30(double *state, double *unused, double *out_496412412853565295) {
  H_30(state, unused, out_496412412853565295);
}
void car_h_26(double *state, double *unused, double *out_8436057024560478219) {
  h_26(state, unused, out_8436057024560478219);
}
void car_H_26(double *state, double *unused, double *out_1282605424107117269) {
  H_26(state, unused, out_1282605424107117269);
}
void car_h_27(double *state, double *unused, double *out_2005415766435312201) {
  h_27(state, unused, out_2005415766435312201);
}
void car_H_27(double *state, double *unused, double *out_2720006484037508512) {
  H_27(state, unused, out_2720006484037508512);
}
void car_h_29(double *state, double *unused, double *out_7531473202986335314) {
  h_29(state, unused, out_7531473202986335314);
}
void car_H_29(double *state, double *unused, double *out_1006643757167957479) {
  H_29(state, unused, out_1006643757167957479);
}
void car_h_28(double *state, double *unused, double *out_8035671487630012762) {
  h_28(state, unused, out_8035671487630012762);
}
void car_H_28(double *state, double *unused, double *out_4075755259901573095) {
  H_28(state, unused, out_4075755259901573095);
}
void car_h_31(double *state, double *unused, double *out_5556457641467274679) {
  h_31(state, unused, out_5556457641467274679);
}
void car_H_31(double *state, double *unused, double *out_656397321873765793) {
  H_31(state, unused, out_656397321873765793);
}
void car_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
void car_set_mass(double x) {
  set_mass(x);
}
void car_set_rotational_inertia(double x) {
  set_rotational_inertia(x);
}
void car_set_center_to_front(double x) {
  set_center_to_front(x);
}
void car_set_center_to_rear(double x) {
  set_center_to_rear(x);
}
void car_set_stiffness_front(double x) {
  set_stiffness_front(x);
}
void car_set_stiffness_rear(double x) {
  set_stiffness_rear(x);
}
}

const EKF car = {
  .name = "car",
  .kinds = { 25, 24, 30, 26, 27, 29, 28, 31 },
  .feature_kinds = {  },
  .f_fun = car_f_fun,
  .F_fun = car_F_fun,
  .err_fun = car_err_fun,
  .inv_err_fun = car_inv_err_fun,
  .H_mod_fun = car_H_mod_fun,
  .predict = car_predict,
  .hs = {
    { 25, car_h_25 },
    { 24, car_h_24 },
    { 30, car_h_30 },
    { 26, car_h_26 },
    { 27, car_h_27 },
    { 29, car_h_29 },
    { 28, car_h_28 },
    { 31, car_h_31 },
  },
  .Hs = {
    { 25, car_H_25 },
    { 24, car_H_24 },
    { 30, car_H_30 },
    { 26, car_H_26 },
    { 27, car_H_27 },
    { 29, car_H_29 },
    { 28, car_H_28 },
    { 31, car_H_31 },
  },
  .updates = {
    { 25, car_update_25 },
    { 24, car_update_24 },
    { 30, car_update_30 },
    { 26, car_update_26 },
    { 27, car_update_27 },
    { 29, car_update_29 },
    { 28, car_update_28 },
    { 31, car_update_31 },
  },
  .Hes = {
  },
  .sets = {
    { "mass", car_set_mass },
    { "rotational_inertia", car_set_rotational_inertia },
    { "center_to_front", car_set_center_to_front },
    { "center_to_rear", car_set_center_to_rear },
    { "stiffness_front", car_set_stiffness_front },
    { "stiffness_rear", car_set_stiffness_rear },
  },
  .extra_routines = {
  },
};

ekf_init(car);
