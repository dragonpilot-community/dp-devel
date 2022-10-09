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
void err_fun(double *nom_x, double *delta_x, double *out_1074556233241015822) {
   out_1074556233241015822[0] = delta_x[0] + nom_x[0];
   out_1074556233241015822[1] = delta_x[1] + nom_x[1];
   out_1074556233241015822[2] = delta_x[2] + nom_x[2];
   out_1074556233241015822[3] = delta_x[3] + nom_x[3];
   out_1074556233241015822[4] = delta_x[4] + nom_x[4];
   out_1074556233241015822[5] = delta_x[5] + nom_x[5];
   out_1074556233241015822[6] = delta_x[6] + nom_x[6];
   out_1074556233241015822[7] = delta_x[7] + nom_x[7];
   out_1074556233241015822[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_2611153403417199822) {
   out_2611153403417199822[0] = -nom_x[0] + true_x[0];
   out_2611153403417199822[1] = -nom_x[1] + true_x[1];
   out_2611153403417199822[2] = -nom_x[2] + true_x[2];
   out_2611153403417199822[3] = -nom_x[3] + true_x[3];
   out_2611153403417199822[4] = -nom_x[4] + true_x[4];
   out_2611153403417199822[5] = -nom_x[5] + true_x[5];
   out_2611153403417199822[6] = -nom_x[6] + true_x[6];
   out_2611153403417199822[7] = -nom_x[7] + true_x[7];
   out_2611153403417199822[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_544146871449396538) {
   out_544146871449396538[0] = 1.0;
   out_544146871449396538[1] = 0;
   out_544146871449396538[2] = 0;
   out_544146871449396538[3] = 0;
   out_544146871449396538[4] = 0;
   out_544146871449396538[5] = 0;
   out_544146871449396538[6] = 0;
   out_544146871449396538[7] = 0;
   out_544146871449396538[8] = 0;
   out_544146871449396538[9] = 0;
   out_544146871449396538[10] = 1.0;
   out_544146871449396538[11] = 0;
   out_544146871449396538[12] = 0;
   out_544146871449396538[13] = 0;
   out_544146871449396538[14] = 0;
   out_544146871449396538[15] = 0;
   out_544146871449396538[16] = 0;
   out_544146871449396538[17] = 0;
   out_544146871449396538[18] = 0;
   out_544146871449396538[19] = 0;
   out_544146871449396538[20] = 1.0;
   out_544146871449396538[21] = 0;
   out_544146871449396538[22] = 0;
   out_544146871449396538[23] = 0;
   out_544146871449396538[24] = 0;
   out_544146871449396538[25] = 0;
   out_544146871449396538[26] = 0;
   out_544146871449396538[27] = 0;
   out_544146871449396538[28] = 0;
   out_544146871449396538[29] = 0;
   out_544146871449396538[30] = 1.0;
   out_544146871449396538[31] = 0;
   out_544146871449396538[32] = 0;
   out_544146871449396538[33] = 0;
   out_544146871449396538[34] = 0;
   out_544146871449396538[35] = 0;
   out_544146871449396538[36] = 0;
   out_544146871449396538[37] = 0;
   out_544146871449396538[38] = 0;
   out_544146871449396538[39] = 0;
   out_544146871449396538[40] = 1.0;
   out_544146871449396538[41] = 0;
   out_544146871449396538[42] = 0;
   out_544146871449396538[43] = 0;
   out_544146871449396538[44] = 0;
   out_544146871449396538[45] = 0;
   out_544146871449396538[46] = 0;
   out_544146871449396538[47] = 0;
   out_544146871449396538[48] = 0;
   out_544146871449396538[49] = 0;
   out_544146871449396538[50] = 1.0;
   out_544146871449396538[51] = 0;
   out_544146871449396538[52] = 0;
   out_544146871449396538[53] = 0;
   out_544146871449396538[54] = 0;
   out_544146871449396538[55] = 0;
   out_544146871449396538[56] = 0;
   out_544146871449396538[57] = 0;
   out_544146871449396538[58] = 0;
   out_544146871449396538[59] = 0;
   out_544146871449396538[60] = 1.0;
   out_544146871449396538[61] = 0;
   out_544146871449396538[62] = 0;
   out_544146871449396538[63] = 0;
   out_544146871449396538[64] = 0;
   out_544146871449396538[65] = 0;
   out_544146871449396538[66] = 0;
   out_544146871449396538[67] = 0;
   out_544146871449396538[68] = 0;
   out_544146871449396538[69] = 0;
   out_544146871449396538[70] = 1.0;
   out_544146871449396538[71] = 0;
   out_544146871449396538[72] = 0;
   out_544146871449396538[73] = 0;
   out_544146871449396538[74] = 0;
   out_544146871449396538[75] = 0;
   out_544146871449396538[76] = 0;
   out_544146871449396538[77] = 0;
   out_544146871449396538[78] = 0;
   out_544146871449396538[79] = 0;
   out_544146871449396538[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_4702214555172441875) {
   out_4702214555172441875[0] = state[0];
   out_4702214555172441875[1] = state[1];
   out_4702214555172441875[2] = state[2];
   out_4702214555172441875[3] = state[3];
   out_4702214555172441875[4] = state[4];
   out_4702214555172441875[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_4702214555172441875[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_4702214555172441875[7] = state[7];
   out_4702214555172441875[8] = state[8];
}
void F_fun(double *state, double dt, double *out_3148325686485299070) {
   out_3148325686485299070[0] = 1;
   out_3148325686485299070[1] = 0;
   out_3148325686485299070[2] = 0;
   out_3148325686485299070[3] = 0;
   out_3148325686485299070[4] = 0;
   out_3148325686485299070[5] = 0;
   out_3148325686485299070[6] = 0;
   out_3148325686485299070[7] = 0;
   out_3148325686485299070[8] = 0;
   out_3148325686485299070[9] = 0;
   out_3148325686485299070[10] = 1;
   out_3148325686485299070[11] = 0;
   out_3148325686485299070[12] = 0;
   out_3148325686485299070[13] = 0;
   out_3148325686485299070[14] = 0;
   out_3148325686485299070[15] = 0;
   out_3148325686485299070[16] = 0;
   out_3148325686485299070[17] = 0;
   out_3148325686485299070[18] = 0;
   out_3148325686485299070[19] = 0;
   out_3148325686485299070[20] = 1;
   out_3148325686485299070[21] = 0;
   out_3148325686485299070[22] = 0;
   out_3148325686485299070[23] = 0;
   out_3148325686485299070[24] = 0;
   out_3148325686485299070[25] = 0;
   out_3148325686485299070[26] = 0;
   out_3148325686485299070[27] = 0;
   out_3148325686485299070[28] = 0;
   out_3148325686485299070[29] = 0;
   out_3148325686485299070[30] = 1;
   out_3148325686485299070[31] = 0;
   out_3148325686485299070[32] = 0;
   out_3148325686485299070[33] = 0;
   out_3148325686485299070[34] = 0;
   out_3148325686485299070[35] = 0;
   out_3148325686485299070[36] = 0;
   out_3148325686485299070[37] = 0;
   out_3148325686485299070[38] = 0;
   out_3148325686485299070[39] = 0;
   out_3148325686485299070[40] = 1;
   out_3148325686485299070[41] = 0;
   out_3148325686485299070[42] = 0;
   out_3148325686485299070[43] = 0;
   out_3148325686485299070[44] = 0;
   out_3148325686485299070[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_3148325686485299070[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_3148325686485299070[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3148325686485299070[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_3148325686485299070[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_3148325686485299070[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_3148325686485299070[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_3148325686485299070[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_3148325686485299070[53] = -9.8000000000000007*dt;
   out_3148325686485299070[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_3148325686485299070[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_3148325686485299070[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3148325686485299070[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3148325686485299070[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_3148325686485299070[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_3148325686485299070[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_3148325686485299070[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_3148325686485299070[62] = 0;
   out_3148325686485299070[63] = 0;
   out_3148325686485299070[64] = 0;
   out_3148325686485299070[65] = 0;
   out_3148325686485299070[66] = 0;
   out_3148325686485299070[67] = 0;
   out_3148325686485299070[68] = 0;
   out_3148325686485299070[69] = 0;
   out_3148325686485299070[70] = 1;
   out_3148325686485299070[71] = 0;
   out_3148325686485299070[72] = 0;
   out_3148325686485299070[73] = 0;
   out_3148325686485299070[74] = 0;
   out_3148325686485299070[75] = 0;
   out_3148325686485299070[76] = 0;
   out_3148325686485299070[77] = 0;
   out_3148325686485299070[78] = 0;
   out_3148325686485299070[79] = 0;
   out_3148325686485299070[80] = 1;
}
void h_25(double *state, double *unused, double *out_1495327568566357005) {
   out_1495327568566357005[0] = state[6];
}
void H_25(double *state, double *unused, double *out_5153499558744102821) {
   out_5153499558744102821[0] = 0;
   out_5153499558744102821[1] = 0;
   out_5153499558744102821[2] = 0;
   out_5153499558744102821[3] = 0;
   out_5153499558744102821[4] = 0;
   out_5153499558744102821[5] = 0;
   out_5153499558744102821[6] = 1;
   out_5153499558744102821[7] = 0;
   out_5153499558744102821[8] = 0;
}
void h_24(double *state, double *unused, double *out_4574568714968761304) {
   out_4574568714968761304[0] = state[4];
   out_4574568714968761304[1] = state[5];
}
void H_24(double *state, double *unused, double *out_2980849959738603255) {
   out_2980849959738603255[0] = 0;
   out_2980849959738603255[1] = 0;
   out_2980849959738603255[2] = 0;
   out_2980849959738603255[3] = 0;
   out_2980849959738603255[4] = 1;
   out_2980849959738603255[5] = 0;
   out_2980849959738603255[6] = 0;
   out_2980849959738603255[7] = 0;
   out_2980849959738603255[8] = 0;
   out_2980849959738603255[9] = 0;
   out_2980849959738603255[10] = 0;
   out_2980849959738603255[11] = 0;
   out_2980849959738603255[12] = 0;
   out_2980849959738603255[13] = 0;
   out_2980849959738603255[14] = 1;
   out_2980849959738603255[15] = 0;
   out_2980849959738603255[16] = 0;
   out_2980849959738603255[17] = 0;
}
void h_30(double *state, double *unused, double *out_2945958961195240172) {
   out_2945958961195240172[0] = state[4];
}
void H_30(double *state, double *unused, double *out_7671832517251351448) {
   out_7671832517251351448[0] = 0;
   out_7671832517251351448[1] = 0;
   out_7671832517251351448[2] = 0;
   out_7671832517251351448[3] = 0;
   out_7671832517251351448[4] = 1;
   out_7671832517251351448[5] = 0;
   out_7671832517251351448[6] = 0;
   out_7671832517251351448[7] = 0;
   out_7671832517251351448[8] = 0;
}
void h_26(double *state, double *unused, double *out_4264950525058981339) {
   out_4264950525058981339[0] = state[7];
}
void H_26(double *state, double *unused, double *out_1411996239870046597) {
   out_1411996239870046597[0] = 0;
   out_1411996239870046597[1] = 0;
   out_1411996239870046597[2] = 0;
   out_1411996239870046597[3] = 0;
   out_1411996239870046597[4] = 0;
   out_1411996239870046597[5] = 0;
   out_1411996239870046597[6] = 0;
   out_1411996239870046597[7] = 1;
   out_1411996239870046597[8] = 0;
}
void h_27(double *state, double *unused, double *out_2657132464078969027) {
   out_2657132464078969027[0] = state[3];
}
void H_27(double *state, double *unused, double *out_8551317485274256951) {
   out_8551317485274256951[0] = 0;
   out_8551317485274256951[1] = 0;
   out_8551317485274256951[2] = 0;
   out_8551317485274256951[3] = 1;
   out_8551317485274256951[4] = 0;
   out_8551317485274256951[5] = 0;
   out_8551317485274256951[6] = 0;
   out_8551317485274256951[7] = 0;
   out_8551317485274256951[8] = 0;
}
void h_29(double *state, double *unused, double *out_2055714306465605473) {
   out_2055714306465605473[0] = state[1];
}
void H_29(double *state, double *unused, double *out_8182063861565743632) {
   out_8182063861565743632[0] = 0;
   out_8182063861565743632[1] = 1;
   out_8182063861565743632[2] = 0;
   out_8182063861565743632[3] = 0;
   out_8182063861565743632[4] = 0;
   out_8182063861565743632[5] = 0;
   out_8182063861565743632[6] = 0;
   out_8182063861565743632[7] = 0;
   out_8182063861565743632[8] = 0;
}
void h_28(double *state, double *unused, double *out_8572763217812831363) {
   out_8572763217812831363[0] = state[0];
}
void H_28(double *state, double *unused, double *out_3099664844496213058) {
   out_3099664844496213058[0] = 1;
   out_3099664844496213058[1] = 0;
   out_3099664844496213058[2] = 0;
   out_3099664844496213058[3] = 0;
   out_3099664844496213058[4] = 0;
   out_3099664844496213058[5] = 0;
   out_3099664844496213058[6] = 0;
   out_3099664844496213058[7] = 0;
   out_3099664844496213058[8] = 0;
}
void h_31(double *state, double *unused, double *out_9070143854389145426) {
   out_9070143854389145426[0] = state[8];
}
void H_31(double *state, double *unused, double *out_785788137636695121) {
   out_785788137636695121[0] = 0;
   out_785788137636695121[1] = 0;
   out_785788137636695121[2] = 0;
   out_785788137636695121[3] = 0;
   out_785788137636695121[4] = 0;
   out_785788137636695121[5] = 0;
   out_785788137636695121[6] = 0;
   out_785788137636695121[7] = 0;
   out_785788137636695121[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_1074556233241015822) {
  err_fun(nom_x, delta_x, out_1074556233241015822);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_2611153403417199822) {
  inv_err_fun(nom_x, true_x, out_2611153403417199822);
}
void car_H_mod_fun(double *state, double *out_544146871449396538) {
  H_mod_fun(state, out_544146871449396538);
}
void car_f_fun(double *state, double dt, double *out_4702214555172441875) {
  f_fun(state,  dt, out_4702214555172441875);
}
void car_F_fun(double *state, double dt, double *out_3148325686485299070) {
  F_fun(state,  dt, out_3148325686485299070);
}
void car_h_25(double *state, double *unused, double *out_1495327568566357005) {
  h_25(state, unused, out_1495327568566357005);
}
void car_H_25(double *state, double *unused, double *out_5153499558744102821) {
  H_25(state, unused, out_5153499558744102821);
}
void car_h_24(double *state, double *unused, double *out_4574568714968761304) {
  h_24(state, unused, out_4574568714968761304);
}
void car_H_24(double *state, double *unused, double *out_2980849959738603255) {
  H_24(state, unused, out_2980849959738603255);
}
void car_h_30(double *state, double *unused, double *out_2945958961195240172) {
  h_30(state, unused, out_2945958961195240172);
}
void car_H_30(double *state, double *unused, double *out_7671832517251351448) {
  H_30(state, unused, out_7671832517251351448);
}
void car_h_26(double *state, double *unused, double *out_4264950525058981339) {
  h_26(state, unused, out_4264950525058981339);
}
void car_H_26(double *state, double *unused, double *out_1411996239870046597) {
  H_26(state, unused, out_1411996239870046597);
}
void car_h_27(double *state, double *unused, double *out_2657132464078969027) {
  h_27(state, unused, out_2657132464078969027);
}
void car_H_27(double *state, double *unused, double *out_8551317485274256951) {
  H_27(state, unused, out_8551317485274256951);
}
void car_h_29(double *state, double *unused, double *out_2055714306465605473) {
  h_29(state, unused, out_2055714306465605473);
}
void car_H_29(double *state, double *unused, double *out_8182063861565743632) {
  H_29(state, unused, out_8182063861565743632);
}
void car_h_28(double *state, double *unused, double *out_8572763217812831363) {
  h_28(state, unused, out_8572763217812831363);
}
void car_H_28(double *state, double *unused, double *out_3099664844496213058) {
  H_28(state, unused, out_3099664844496213058);
}
void car_h_31(double *state, double *unused, double *out_9070143854389145426) {
  h_31(state, unused, out_9070143854389145426);
}
void car_H_31(double *state, double *unused, double *out_785788137636695121) {
  H_31(state, unused, out_785788137636695121);
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
