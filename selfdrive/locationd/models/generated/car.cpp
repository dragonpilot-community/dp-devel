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
void err_fun(double *nom_x, double *delta_x, double *out_4218563854107170996) {
   out_4218563854107170996[0] = delta_x[0] + nom_x[0];
   out_4218563854107170996[1] = delta_x[1] + nom_x[1];
   out_4218563854107170996[2] = delta_x[2] + nom_x[2];
   out_4218563854107170996[3] = delta_x[3] + nom_x[3];
   out_4218563854107170996[4] = delta_x[4] + nom_x[4];
   out_4218563854107170996[5] = delta_x[5] + nom_x[5];
   out_4218563854107170996[6] = delta_x[6] + nom_x[6];
   out_4218563854107170996[7] = delta_x[7] + nom_x[7];
   out_4218563854107170996[8] = delta_x[8] + nom_x[8];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_1544528291709365585) {
   out_1544528291709365585[0] = -nom_x[0] + true_x[0];
   out_1544528291709365585[1] = -nom_x[1] + true_x[1];
   out_1544528291709365585[2] = -nom_x[2] + true_x[2];
   out_1544528291709365585[3] = -nom_x[3] + true_x[3];
   out_1544528291709365585[4] = -nom_x[4] + true_x[4];
   out_1544528291709365585[5] = -nom_x[5] + true_x[5];
   out_1544528291709365585[6] = -nom_x[6] + true_x[6];
   out_1544528291709365585[7] = -nom_x[7] + true_x[7];
   out_1544528291709365585[8] = -nom_x[8] + true_x[8];
}
void H_mod_fun(double *state, double *out_3035036435099682035) {
   out_3035036435099682035[0] = 1.0;
   out_3035036435099682035[1] = 0;
   out_3035036435099682035[2] = 0;
   out_3035036435099682035[3] = 0;
   out_3035036435099682035[4] = 0;
   out_3035036435099682035[5] = 0;
   out_3035036435099682035[6] = 0;
   out_3035036435099682035[7] = 0;
   out_3035036435099682035[8] = 0;
   out_3035036435099682035[9] = 0;
   out_3035036435099682035[10] = 1.0;
   out_3035036435099682035[11] = 0;
   out_3035036435099682035[12] = 0;
   out_3035036435099682035[13] = 0;
   out_3035036435099682035[14] = 0;
   out_3035036435099682035[15] = 0;
   out_3035036435099682035[16] = 0;
   out_3035036435099682035[17] = 0;
   out_3035036435099682035[18] = 0;
   out_3035036435099682035[19] = 0;
   out_3035036435099682035[20] = 1.0;
   out_3035036435099682035[21] = 0;
   out_3035036435099682035[22] = 0;
   out_3035036435099682035[23] = 0;
   out_3035036435099682035[24] = 0;
   out_3035036435099682035[25] = 0;
   out_3035036435099682035[26] = 0;
   out_3035036435099682035[27] = 0;
   out_3035036435099682035[28] = 0;
   out_3035036435099682035[29] = 0;
   out_3035036435099682035[30] = 1.0;
   out_3035036435099682035[31] = 0;
   out_3035036435099682035[32] = 0;
   out_3035036435099682035[33] = 0;
   out_3035036435099682035[34] = 0;
   out_3035036435099682035[35] = 0;
   out_3035036435099682035[36] = 0;
   out_3035036435099682035[37] = 0;
   out_3035036435099682035[38] = 0;
   out_3035036435099682035[39] = 0;
   out_3035036435099682035[40] = 1.0;
   out_3035036435099682035[41] = 0;
   out_3035036435099682035[42] = 0;
   out_3035036435099682035[43] = 0;
   out_3035036435099682035[44] = 0;
   out_3035036435099682035[45] = 0;
   out_3035036435099682035[46] = 0;
   out_3035036435099682035[47] = 0;
   out_3035036435099682035[48] = 0;
   out_3035036435099682035[49] = 0;
   out_3035036435099682035[50] = 1.0;
   out_3035036435099682035[51] = 0;
   out_3035036435099682035[52] = 0;
   out_3035036435099682035[53] = 0;
   out_3035036435099682035[54] = 0;
   out_3035036435099682035[55] = 0;
   out_3035036435099682035[56] = 0;
   out_3035036435099682035[57] = 0;
   out_3035036435099682035[58] = 0;
   out_3035036435099682035[59] = 0;
   out_3035036435099682035[60] = 1.0;
   out_3035036435099682035[61] = 0;
   out_3035036435099682035[62] = 0;
   out_3035036435099682035[63] = 0;
   out_3035036435099682035[64] = 0;
   out_3035036435099682035[65] = 0;
   out_3035036435099682035[66] = 0;
   out_3035036435099682035[67] = 0;
   out_3035036435099682035[68] = 0;
   out_3035036435099682035[69] = 0;
   out_3035036435099682035[70] = 1.0;
   out_3035036435099682035[71] = 0;
   out_3035036435099682035[72] = 0;
   out_3035036435099682035[73] = 0;
   out_3035036435099682035[74] = 0;
   out_3035036435099682035[75] = 0;
   out_3035036435099682035[76] = 0;
   out_3035036435099682035[77] = 0;
   out_3035036435099682035[78] = 0;
   out_3035036435099682035[79] = 0;
   out_3035036435099682035[80] = 1.0;
}
void f_fun(double *state, double dt, double *out_1150570653314300042) {
   out_1150570653314300042[0] = state[0];
   out_1150570653314300042[1] = state[1];
   out_1150570653314300042[2] = state[2];
   out_1150570653314300042[3] = state[3];
   out_1150570653314300042[4] = state[4];
   out_1150570653314300042[5] = dt*((-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]))*state[6] - 9.8000000000000007*state[8] + stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*state[1]) + (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*state[4])) + state[5];
   out_1150570653314300042[6] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*state[4])) + state[6];
   out_1150570653314300042[7] = state[7];
   out_1150570653314300042[8] = state[8];
}
void F_fun(double *state, double dt, double *out_5640151155408725370) {
   out_5640151155408725370[0] = 1;
   out_5640151155408725370[1] = 0;
   out_5640151155408725370[2] = 0;
   out_5640151155408725370[3] = 0;
   out_5640151155408725370[4] = 0;
   out_5640151155408725370[5] = 0;
   out_5640151155408725370[6] = 0;
   out_5640151155408725370[7] = 0;
   out_5640151155408725370[8] = 0;
   out_5640151155408725370[9] = 0;
   out_5640151155408725370[10] = 1;
   out_5640151155408725370[11] = 0;
   out_5640151155408725370[12] = 0;
   out_5640151155408725370[13] = 0;
   out_5640151155408725370[14] = 0;
   out_5640151155408725370[15] = 0;
   out_5640151155408725370[16] = 0;
   out_5640151155408725370[17] = 0;
   out_5640151155408725370[18] = 0;
   out_5640151155408725370[19] = 0;
   out_5640151155408725370[20] = 1;
   out_5640151155408725370[21] = 0;
   out_5640151155408725370[22] = 0;
   out_5640151155408725370[23] = 0;
   out_5640151155408725370[24] = 0;
   out_5640151155408725370[25] = 0;
   out_5640151155408725370[26] = 0;
   out_5640151155408725370[27] = 0;
   out_5640151155408725370[28] = 0;
   out_5640151155408725370[29] = 0;
   out_5640151155408725370[30] = 1;
   out_5640151155408725370[31] = 0;
   out_5640151155408725370[32] = 0;
   out_5640151155408725370[33] = 0;
   out_5640151155408725370[34] = 0;
   out_5640151155408725370[35] = 0;
   out_5640151155408725370[36] = 0;
   out_5640151155408725370[37] = 0;
   out_5640151155408725370[38] = 0;
   out_5640151155408725370[39] = 0;
   out_5640151155408725370[40] = 1;
   out_5640151155408725370[41] = 0;
   out_5640151155408725370[42] = 0;
   out_5640151155408725370[43] = 0;
   out_5640151155408725370[44] = 0;
   out_5640151155408725370[45] = dt*(stiffness_front*(-state[2] - state[3] + state[7])/(mass*state[1]) + (-stiffness_front - stiffness_rear)*state[5]/(mass*state[4]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[6]/(mass*state[4]));
   out_5640151155408725370[46] = -dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(mass*pow(state[1], 2));
   out_5640151155408725370[47] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5640151155408725370[48] = -dt*stiffness_front*state[0]/(mass*state[1]);
   out_5640151155408725370[49] = dt*((-1 - (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*pow(state[4], 2)))*state[6] - (-stiffness_front*state[0] - stiffness_rear*state[0])*state[5]/(mass*pow(state[4], 2)));
   out_5640151155408725370[50] = dt*(-stiffness_front*state[0] - stiffness_rear*state[0])/(mass*state[4]) + 1;
   out_5640151155408725370[51] = dt*(-state[4] + (-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(mass*state[4]));
   out_5640151155408725370[52] = dt*stiffness_front*state[0]/(mass*state[1]);
   out_5640151155408725370[53] = -9.8000000000000007*dt;
   out_5640151155408725370[54] = dt*(center_to_front*stiffness_front*(-state[2] - state[3] + state[7])/(rotational_inertia*state[1]) + (-center_to_front*stiffness_front + center_to_rear*stiffness_rear)*state[5]/(rotational_inertia*state[4]) + (-pow(center_to_front, 2)*stiffness_front - pow(center_to_rear, 2)*stiffness_rear)*state[6]/(rotational_inertia*state[4]));
   out_5640151155408725370[55] = -center_to_front*dt*stiffness_front*(-state[2] - state[3] + state[7])*state[0]/(rotational_inertia*pow(state[1], 2));
   out_5640151155408725370[56] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5640151155408725370[57] = -center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5640151155408725370[58] = dt*(-(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])*state[5]/(rotational_inertia*pow(state[4], 2)) - (-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])*state[6]/(rotational_inertia*pow(state[4], 2)));
   out_5640151155408725370[59] = dt*(-center_to_front*stiffness_front*state[0] + center_to_rear*stiffness_rear*state[0])/(rotational_inertia*state[4]);
   out_5640151155408725370[60] = dt*(-pow(center_to_front, 2)*stiffness_front*state[0] - pow(center_to_rear, 2)*stiffness_rear*state[0])/(rotational_inertia*state[4]) + 1;
   out_5640151155408725370[61] = center_to_front*dt*stiffness_front*state[0]/(rotational_inertia*state[1]);
   out_5640151155408725370[62] = 0;
   out_5640151155408725370[63] = 0;
   out_5640151155408725370[64] = 0;
   out_5640151155408725370[65] = 0;
   out_5640151155408725370[66] = 0;
   out_5640151155408725370[67] = 0;
   out_5640151155408725370[68] = 0;
   out_5640151155408725370[69] = 0;
   out_5640151155408725370[70] = 1;
   out_5640151155408725370[71] = 0;
   out_5640151155408725370[72] = 0;
   out_5640151155408725370[73] = 0;
   out_5640151155408725370[74] = 0;
   out_5640151155408725370[75] = 0;
   out_5640151155408725370[76] = 0;
   out_5640151155408725370[77] = 0;
   out_5640151155408725370[78] = 0;
   out_5640151155408725370[79] = 0;
   out_5640151155408725370[80] = 1;
}
void h_25(double *state, double *unused, double *out_4669821610217864848) {
   out_4669821610217864848[0] = state[6];
}
void H_25(double *state, double *unused, double *out_6258512777106223790) {
   out_6258512777106223790[0] = 0;
   out_6258512777106223790[1] = 0;
   out_6258512777106223790[2] = 0;
   out_6258512777106223790[3] = 0;
   out_6258512777106223790[4] = 0;
   out_6258512777106223790[5] = 0;
   out_6258512777106223790[6] = 1;
   out_6258512777106223790[7] = 0;
   out_6258512777106223790[8] = 0;
}
void h_24(double *state, double *unused, double *out_5751839562146525567) {
   out_5751839562146525567[0] = state[4];
   out_5751839562146525567[1] = state[5];
}
void H_24(double *state, double *unused, double *out_4745556703614611506) {
   out_4745556703614611506[0] = 0;
   out_4745556703614611506[1] = 0;
   out_4745556703614611506[2] = 0;
   out_4745556703614611506[3] = 0;
   out_4745556703614611506[4] = 1;
   out_4745556703614611506[5] = 0;
   out_4745556703614611506[6] = 0;
   out_4745556703614611506[7] = 0;
   out_4745556703614611506[8] = 0;
   out_4745556703614611506[9] = 0;
   out_4745556703614611506[10] = 0;
   out_4745556703614611506[11] = 0;
   out_4745556703614611506[12] = 0;
   out_4745556703614611506[13] = 0;
   out_4745556703614611506[14] = 1;
   out_4745556703614611506[15] = 0;
   out_4745556703614611506[16] = 0;
   out_4745556703614611506[17] = 0;
}
void h_30(double *state, double *unused, double *out_4945015672502370737) {
   out_4945015672502370737[0] = state[4];
}
void H_30(double *state, double *unused, double *out_6387851724249463860) {
   out_6387851724249463860[0] = 0;
   out_6387851724249463860[1] = 0;
   out_6387851724249463860[2] = 0;
   out_6387851724249463860[3] = 0;
   out_6387851724249463860[4] = 1;
   out_6387851724249463860[5] = 0;
   out_6387851724249463860[6] = 0;
   out_6387851724249463860[7] = 0;
   out_6387851724249463860[8] = 0;
}
void h_26(double *state, double *unused, double *out_933269815056344351) {
   out_933269815056344351[0] = state[7];
}
void H_26(double *state, double *unused, double *out_8446727977729271602) {
   out_8446727977729271602[0] = 0;
   out_8446727977729271602[1] = 0;
   out_8446727977729271602[2] = 0;
   out_8446727977729271602[3] = 0;
   out_8446727977729271602[4] = 0;
   out_8446727977729271602[5] = 0;
   out_8446727977729271602[6] = 0;
   out_8446727977729271602[7] = 1;
   out_8446727977729271602[8] = 0;
}
void h_27(double *state, double *unused, double *out_4756379051051210326) {
   out_4756379051051210326[0] = state[3];
}
void H_27(double *state, double *unused, double *out_8562615036049888771) {
   out_8562615036049888771[0] = 0;
   out_8562615036049888771[1] = 0;
   out_8562615036049888771[2] = 0;
   out_8562615036049888771[3] = 1;
   out_8562615036049888771[4] = 0;
   out_8562615036049888771[5] = 0;
   out_8562615036049888771[6] = 0;
   out_8562615036049888771[7] = 0;
   out_8562615036049888771[8] = 0;
}
void h_29(double *state, double *unused, double *out_4913565719402339471) {
   out_4913565719402339471[0] = state[1];
}
void H_29(double *state, double *unused, double *out_8170766310790111812) {
   out_8170766310790111812[0] = 0;
   out_8170766310790111812[1] = 1;
   out_8170766310790111812[2] = 0;
   out_8170766310790111812[3] = 0;
   out_8170766310790111812[4] = 0;
   out_8170766310790111812[5] = 0;
   out_8170766310790111812[6] = 0;
   out_8170766310790111812[7] = 0;
   out_8170766310790111812[8] = 0;
}
void h_28(double *state, double *unused, double *out_7291323505244492144) {
   out_7291323505244492144[0] = state[0];
}
void H_28(double *state, double *unused, double *out_3088367293720581238) {
   out_3088367293720581238[0] = 1;
   out_3088367293720581238[1] = 0;
   out_3088367293720581238[2] = 0;
   out_3088367293720581238[3] = 0;
   out_3088367293720581238[4] = 0;
   out_3088367293720581238[5] = 0;
   out_3088367293720581238[6] = 0;
   out_3088367293720581238[7] = 0;
   out_3088367293720581238[8] = 0;
}
void h_31(double *state, double *unused, double *out_4827008278568993993) {
   out_4827008278568993993[0] = state[8];
}
void H_31(double *state, double *unused, double *out_6227866815229263362) {
   out_6227866815229263362[0] = 0;
   out_6227866815229263362[1] = 0;
   out_6227866815229263362[2] = 0;
   out_6227866815229263362[3] = 0;
   out_6227866815229263362[4] = 0;
   out_6227866815229263362[5] = 0;
   out_6227866815229263362[6] = 0;
   out_6227866815229263362[7] = 0;
   out_6227866815229263362[8] = 1;
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
void car_err_fun(double *nom_x, double *delta_x, double *out_4218563854107170996) {
  err_fun(nom_x, delta_x, out_4218563854107170996);
}
void car_inv_err_fun(double *nom_x, double *true_x, double *out_1544528291709365585) {
  inv_err_fun(nom_x, true_x, out_1544528291709365585);
}
void car_H_mod_fun(double *state, double *out_3035036435099682035) {
  H_mod_fun(state, out_3035036435099682035);
}
void car_f_fun(double *state, double dt, double *out_1150570653314300042) {
  f_fun(state,  dt, out_1150570653314300042);
}
void car_F_fun(double *state, double dt, double *out_5640151155408725370) {
  F_fun(state,  dt, out_5640151155408725370);
}
void car_h_25(double *state, double *unused, double *out_4669821610217864848) {
  h_25(state, unused, out_4669821610217864848);
}
void car_H_25(double *state, double *unused, double *out_6258512777106223790) {
  H_25(state, unused, out_6258512777106223790);
}
void car_h_24(double *state, double *unused, double *out_5751839562146525567) {
  h_24(state, unused, out_5751839562146525567);
}
void car_H_24(double *state, double *unused, double *out_4745556703614611506) {
  H_24(state, unused, out_4745556703614611506);
}
void car_h_30(double *state, double *unused, double *out_4945015672502370737) {
  h_30(state, unused, out_4945015672502370737);
}
void car_H_30(double *state, double *unused, double *out_6387851724249463860) {
  H_30(state, unused, out_6387851724249463860);
}
void car_h_26(double *state, double *unused, double *out_933269815056344351) {
  h_26(state, unused, out_933269815056344351);
}
void car_H_26(double *state, double *unused, double *out_8446727977729271602) {
  H_26(state, unused, out_8446727977729271602);
}
void car_h_27(double *state, double *unused, double *out_4756379051051210326) {
  h_27(state, unused, out_4756379051051210326);
}
void car_H_27(double *state, double *unused, double *out_8562615036049888771) {
  H_27(state, unused, out_8562615036049888771);
}
void car_h_29(double *state, double *unused, double *out_4913565719402339471) {
  h_29(state, unused, out_4913565719402339471);
}
void car_H_29(double *state, double *unused, double *out_8170766310790111812) {
  H_29(state, unused, out_8170766310790111812);
}
void car_h_28(double *state, double *unused, double *out_7291323505244492144) {
  h_28(state, unused, out_7291323505244492144);
}
void car_H_28(double *state, double *unused, double *out_3088367293720581238) {
  H_28(state, unused, out_3088367293720581238);
}
void car_h_31(double *state, double *unused, double *out_4827008278568993993) {
  h_31(state, unused, out_4827008278568993993);
}
void car_H_31(double *state, double *unused, double *out_6227866815229263362) {
  H_31(state, unused, out_6227866815229263362);
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
