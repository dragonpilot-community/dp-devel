#include "gnss.h"

namespace {
#define DIM 11
#define EDIM 11
#define MEDIM 11
typedef void (*Hfun)(double *, double *, double *);
const static double MAHA_THRESH_6 = 3.8414588206941227;
const static double MAHA_THRESH_20 = 3.8414588206941227;
const static double MAHA_THRESH_7 = 3.8414588206941227;
const static double MAHA_THRESH_21 = 3.8414588206941227;

/******************************************************************************
 *                      Code generated with SymPy 1.11.1                      *
 *                                                                            *
 *              See http://www.sympy.org/ for more information.               *
 *                                                                            *
 *                         This file is part of 'ekf'                         *
 ******************************************************************************/
void err_fun(double *nom_x, double *delta_x, double *out_4347860218965336784) {
   out_4347860218965336784[0] = delta_x[0] + nom_x[0];
   out_4347860218965336784[1] = delta_x[1] + nom_x[1];
   out_4347860218965336784[2] = delta_x[2] + nom_x[2];
   out_4347860218965336784[3] = delta_x[3] + nom_x[3];
   out_4347860218965336784[4] = delta_x[4] + nom_x[4];
   out_4347860218965336784[5] = delta_x[5] + nom_x[5];
   out_4347860218965336784[6] = delta_x[6] + nom_x[6];
   out_4347860218965336784[7] = delta_x[7] + nom_x[7];
   out_4347860218965336784[8] = delta_x[8] + nom_x[8];
   out_4347860218965336784[9] = delta_x[9] + nom_x[9];
   out_4347860218965336784[10] = delta_x[10] + nom_x[10];
}
void inv_err_fun(double *nom_x, double *true_x, double *out_291039236896220829) {
   out_291039236896220829[0] = -nom_x[0] + true_x[0];
   out_291039236896220829[1] = -nom_x[1] + true_x[1];
   out_291039236896220829[2] = -nom_x[2] + true_x[2];
   out_291039236896220829[3] = -nom_x[3] + true_x[3];
   out_291039236896220829[4] = -nom_x[4] + true_x[4];
   out_291039236896220829[5] = -nom_x[5] + true_x[5];
   out_291039236896220829[6] = -nom_x[6] + true_x[6];
   out_291039236896220829[7] = -nom_x[7] + true_x[7];
   out_291039236896220829[8] = -nom_x[8] + true_x[8];
   out_291039236896220829[9] = -nom_x[9] + true_x[9];
   out_291039236896220829[10] = -nom_x[10] + true_x[10];
}
void H_mod_fun(double *state, double *out_9211819402308797020) {
   out_9211819402308797020[0] = 1.0;
   out_9211819402308797020[1] = 0;
   out_9211819402308797020[2] = 0;
   out_9211819402308797020[3] = 0;
   out_9211819402308797020[4] = 0;
   out_9211819402308797020[5] = 0;
   out_9211819402308797020[6] = 0;
   out_9211819402308797020[7] = 0;
   out_9211819402308797020[8] = 0;
   out_9211819402308797020[9] = 0;
   out_9211819402308797020[10] = 0;
   out_9211819402308797020[11] = 0;
   out_9211819402308797020[12] = 1.0;
   out_9211819402308797020[13] = 0;
   out_9211819402308797020[14] = 0;
   out_9211819402308797020[15] = 0;
   out_9211819402308797020[16] = 0;
   out_9211819402308797020[17] = 0;
   out_9211819402308797020[18] = 0;
   out_9211819402308797020[19] = 0;
   out_9211819402308797020[20] = 0;
   out_9211819402308797020[21] = 0;
   out_9211819402308797020[22] = 0;
   out_9211819402308797020[23] = 0;
   out_9211819402308797020[24] = 1.0;
   out_9211819402308797020[25] = 0;
   out_9211819402308797020[26] = 0;
   out_9211819402308797020[27] = 0;
   out_9211819402308797020[28] = 0;
   out_9211819402308797020[29] = 0;
   out_9211819402308797020[30] = 0;
   out_9211819402308797020[31] = 0;
   out_9211819402308797020[32] = 0;
   out_9211819402308797020[33] = 0;
   out_9211819402308797020[34] = 0;
   out_9211819402308797020[35] = 0;
   out_9211819402308797020[36] = 1.0;
   out_9211819402308797020[37] = 0;
   out_9211819402308797020[38] = 0;
   out_9211819402308797020[39] = 0;
   out_9211819402308797020[40] = 0;
   out_9211819402308797020[41] = 0;
   out_9211819402308797020[42] = 0;
   out_9211819402308797020[43] = 0;
   out_9211819402308797020[44] = 0;
   out_9211819402308797020[45] = 0;
   out_9211819402308797020[46] = 0;
   out_9211819402308797020[47] = 0;
   out_9211819402308797020[48] = 1.0;
   out_9211819402308797020[49] = 0;
   out_9211819402308797020[50] = 0;
   out_9211819402308797020[51] = 0;
   out_9211819402308797020[52] = 0;
   out_9211819402308797020[53] = 0;
   out_9211819402308797020[54] = 0;
   out_9211819402308797020[55] = 0;
   out_9211819402308797020[56] = 0;
   out_9211819402308797020[57] = 0;
   out_9211819402308797020[58] = 0;
   out_9211819402308797020[59] = 0;
   out_9211819402308797020[60] = 1.0;
   out_9211819402308797020[61] = 0;
   out_9211819402308797020[62] = 0;
   out_9211819402308797020[63] = 0;
   out_9211819402308797020[64] = 0;
   out_9211819402308797020[65] = 0;
   out_9211819402308797020[66] = 0;
   out_9211819402308797020[67] = 0;
   out_9211819402308797020[68] = 0;
   out_9211819402308797020[69] = 0;
   out_9211819402308797020[70] = 0;
   out_9211819402308797020[71] = 0;
   out_9211819402308797020[72] = 1.0;
   out_9211819402308797020[73] = 0;
   out_9211819402308797020[74] = 0;
   out_9211819402308797020[75] = 0;
   out_9211819402308797020[76] = 0;
   out_9211819402308797020[77] = 0;
   out_9211819402308797020[78] = 0;
   out_9211819402308797020[79] = 0;
   out_9211819402308797020[80] = 0;
   out_9211819402308797020[81] = 0;
   out_9211819402308797020[82] = 0;
   out_9211819402308797020[83] = 0;
   out_9211819402308797020[84] = 1.0;
   out_9211819402308797020[85] = 0;
   out_9211819402308797020[86] = 0;
   out_9211819402308797020[87] = 0;
   out_9211819402308797020[88] = 0;
   out_9211819402308797020[89] = 0;
   out_9211819402308797020[90] = 0;
   out_9211819402308797020[91] = 0;
   out_9211819402308797020[92] = 0;
   out_9211819402308797020[93] = 0;
   out_9211819402308797020[94] = 0;
   out_9211819402308797020[95] = 0;
   out_9211819402308797020[96] = 1.0;
   out_9211819402308797020[97] = 0;
   out_9211819402308797020[98] = 0;
   out_9211819402308797020[99] = 0;
   out_9211819402308797020[100] = 0;
   out_9211819402308797020[101] = 0;
   out_9211819402308797020[102] = 0;
   out_9211819402308797020[103] = 0;
   out_9211819402308797020[104] = 0;
   out_9211819402308797020[105] = 0;
   out_9211819402308797020[106] = 0;
   out_9211819402308797020[107] = 0;
   out_9211819402308797020[108] = 1.0;
   out_9211819402308797020[109] = 0;
   out_9211819402308797020[110] = 0;
   out_9211819402308797020[111] = 0;
   out_9211819402308797020[112] = 0;
   out_9211819402308797020[113] = 0;
   out_9211819402308797020[114] = 0;
   out_9211819402308797020[115] = 0;
   out_9211819402308797020[116] = 0;
   out_9211819402308797020[117] = 0;
   out_9211819402308797020[118] = 0;
   out_9211819402308797020[119] = 0;
   out_9211819402308797020[120] = 1.0;
}
void f_fun(double *state, double dt, double *out_4796346524927390369) {
   out_4796346524927390369[0] = dt*state[3] + state[0];
   out_4796346524927390369[1] = dt*state[4] + state[1];
   out_4796346524927390369[2] = dt*state[5] + state[2];
   out_4796346524927390369[3] = state[3];
   out_4796346524927390369[4] = state[4];
   out_4796346524927390369[5] = state[5];
   out_4796346524927390369[6] = dt*state[7] + state[6];
   out_4796346524927390369[7] = dt*state[8] + state[7];
   out_4796346524927390369[8] = state[8];
   out_4796346524927390369[9] = state[9];
   out_4796346524927390369[10] = state[10];
}
void F_fun(double *state, double dt, double *out_4441170918248855648) {
   out_4441170918248855648[0] = 1;
   out_4441170918248855648[1] = 0;
   out_4441170918248855648[2] = 0;
   out_4441170918248855648[3] = dt;
   out_4441170918248855648[4] = 0;
   out_4441170918248855648[5] = 0;
   out_4441170918248855648[6] = 0;
   out_4441170918248855648[7] = 0;
   out_4441170918248855648[8] = 0;
   out_4441170918248855648[9] = 0;
   out_4441170918248855648[10] = 0;
   out_4441170918248855648[11] = 0;
   out_4441170918248855648[12] = 1;
   out_4441170918248855648[13] = 0;
   out_4441170918248855648[14] = 0;
   out_4441170918248855648[15] = dt;
   out_4441170918248855648[16] = 0;
   out_4441170918248855648[17] = 0;
   out_4441170918248855648[18] = 0;
   out_4441170918248855648[19] = 0;
   out_4441170918248855648[20] = 0;
   out_4441170918248855648[21] = 0;
   out_4441170918248855648[22] = 0;
   out_4441170918248855648[23] = 0;
   out_4441170918248855648[24] = 1;
   out_4441170918248855648[25] = 0;
   out_4441170918248855648[26] = 0;
   out_4441170918248855648[27] = dt;
   out_4441170918248855648[28] = 0;
   out_4441170918248855648[29] = 0;
   out_4441170918248855648[30] = 0;
   out_4441170918248855648[31] = 0;
   out_4441170918248855648[32] = 0;
   out_4441170918248855648[33] = 0;
   out_4441170918248855648[34] = 0;
   out_4441170918248855648[35] = 0;
   out_4441170918248855648[36] = 1;
   out_4441170918248855648[37] = 0;
   out_4441170918248855648[38] = 0;
   out_4441170918248855648[39] = 0;
   out_4441170918248855648[40] = 0;
   out_4441170918248855648[41] = 0;
   out_4441170918248855648[42] = 0;
   out_4441170918248855648[43] = 0;
   out_4441170918248855648[44] = 0;
   out_4441170918248855648[45] = 0;
   out_4441170918248855648[46] = 0;
   out_4441170918248855648[47] = 0;
   out_4441170918248855648[48] = 1;
   out_4441170918248855648[49] = 0;
   out_4441170918248855648[50] = 0;
   out_4441170918248855648[51] = 0;
   out_4441170918248855648[52] = 0;
   out_4441170918248855648[53] = 0;
   out_4441170918248855648[54] = 0;
   out_4441170918248855648[55] = 0;
   out_4441170918248855648[56] = 0;
   out_4441170918248855648[57] = 0;
   out_4441170918248855648[58] = 0;
   out_4441170918248855648[59] = 0;
   out_4441170918248855648[60] = 1;
   out_4441170918248855648[61] = 0;
   out_4441170918248855648[62] = 0;
   out_4441170918248855648[63] = 0;
   out_4441170918248855648[64] = 0;
   out_4441170918248855648[65] = 0;
   out_4441170918248855648[66] = 0;
   out_4441170918248855648[67] = 0;
   out_4441170918248855648[68] = 0;
   out_4441170918248855648[69] = 0;
   out_4441170918248855648[70] = 0;
   out_4441170918248855648[71] = 0;
   out_4441170918248855648[72] = 1;
   out_4441170918248855648[73] = dt;
   out_4441170918248855648[74] = 0;
   out_4441170918248855648[75] = 0;
   out_4441170918248855648[76] = 0;
   out_4441170918248855648[77] = 0;
   out_4441170918248855648[78] = 0;
   out_4441170918248855648[79] = 0;
   out_4441170918248855648[80] = 0;
   out_4441170918248855648[81] = 0;
   out_4441170918248855648[82] = 0;
   out_4441170918248855648[83] = 0;
   out_4441170918248855648[84] = 1;
   out_4441170918248855648[85] = dt;
   out_4441170918248855648[86] = 0;
   out_4441170918248855648[87] = 0;
   out_4441170918248855648[88] = 0;
   out_4441170918248855648[89] = 0;
   out_4441170918248855648[90] = 0;
   out_4441170918248855648[91] = 0;
   out_4441170918248855648[92] = 0;
   out_4441170918248855648[93] = 0;
   out_4441170918248855648[94] = 0;
   out_4441170918248855648[95] = 0;
   out_4441170918248855648[96] = 1;
   out_4441170918248855648[97] = 0;
   out_4441170918248855648[98] = 0;
   out_4441170918248855648[99] = 0;
   out_4441170918248855648[100] = 0;
   out_4441170918248855648[101] = 0;
   out_4441170918248855648[102] = 0;
   out_4441170918248855648[103] = 0;
   out_4441170918248855648[104] = 0;
   out_4441170918248855648[105] = 0;
   out_4441170918248855648[106] = 0;
   out_4441170918248855648[107] = 0;
   out_4441170918248855648[108] = 1;
   out_4441170918248855648[109] = 0;
   out_4441170918248855648[110] = 0;
   out_4441170918248855648[111] = 0;
   out_4441170918248855648[112] = 0;
   out_4441170918248855648[113] = 0;
   out_4441170918248855648[114] = 0;
   out_4441170918248855648[115] = 0;
   out_4441170918248855648[116] = 0;
   out_4441170918248855648[117] = 0;
   out_4441170918248855648[118] = 0;
   out_4441170918248855648[119] = 0;
   out_4441170918248855648[120] = 1;
}
void h_6(double *state, double *sat_pos, double *out_3354259565467953634) {
   out_3354259565467953634[0] = sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2)) + state[6];
}
void H_6(double *state, double *sat_pos, double *out_5049104898748510284) {
   out_5049104898748510284[0] = (-sat_pos[0] + state[0])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_5049104898748510284[1] = (-sat_pos[1] + state[1])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_5049104898748510284[2] = (-sat_pos[2] + state[2])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_5049104898748510284[3] = 0;
   out_5049104898748510284[4] = 0;
   out_5049104898748510284[5] = 0;
   out_5049104898748510284[6] = 1;
   out_5049104898748510284[7] = 0;
   out_5049104898748510284[8] = 0;
   out_5049104898748510284[9] = 0;
   out_5049104898748510284[10] = 0;
}
void h_20(double *state, double *sat_pos, double *out_4970306618701812397) {
   out_4970306618701812397[0] = sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2)) + sat_pos[3]*state[10] + state[6] + state[9];
}
void H_20(double *state, double *sat_pos, double *out_3980112008404051449) {
   out_3980112008404051449[0] = (-sat_pos[0] + state[0])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_3980112008404051449[1] = (-sat_pos[1] + state[1])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_3980112008404051449[2] = (-sat_pos[2] + state[2])/sqrt(pow(-sat_pos[0] + state[0], 2) + pow(-sat_pos[1] + state[1], 2) + pow(-sat_pos[2] + state[2], 2));
   out_3980112008404051449[3] = 0;
   out_3980112008404051449[4] = 0;
   out_3980112008404051449[5] = 0;
   out_3980112008404051449[6] = 1;
   out_3980112008404051449[7] = 0;
   out_3980112008404051449[8] = 0;
   out_3980112008404051449[9] = 1;
   out_3980112008404051449[10] = sat_pos[3];
}
void h_7(double *state, double *sat_pos_vel, double *out_1461985868216609017) {
   out_1461985868216609017[0] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + state[7];
}
void H_7(double *state, double *sat_pos_vel, double *out_6914887183114622221) {
   out_6914887183114622221[0] = pow(sat_pos_vel[0] - state[0], 2)*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[1] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[1] - state[1], 2)*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[2] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[2] - state[2], 2)*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[3] = -(sat_pos_vel[0] - state[0])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[4] = -(sat_pos_vel[1] - state[1])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[5] = -(sat_pos_vel[2] - state[2])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[6] = 0;
   out_6914887183114622221[7] = 1;
   out_6914887183114622221[8] = 0;
   out_6914887183114622221[9] = 0;
   out_6914887183114622221[10] = 0;
}
void h_21(double *state, double *sat_pos_vel, double *out_1461985868216609017) {
   out_1461985868216609017[0] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + (sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2)) + state[7];
}
void H_21(double *state, double *sat_pos_vel, double *out_6914887183114622221) {
   out_6914887183114622221[0] = pow(sat_pos_vel[0] - state[0], 2)*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[3] - state[3])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[1] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[1] - state[1])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[1] - state[1], 2)*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[4] - state[4])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[2] = (sat_pos_vel[0] - state[0])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[3] - state[3])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + (sat_pos_vel[1] - state[1])*(sat_pos_vel[2] - state[2])*(sat_pos_vel[4] - state[4])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) + pow(sat_pos_vel[2] - state[2], 2)*(sat_pos_vel[5] - state[5])/pow(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2), 3.0/2.0) - (sat_pos_vel[5] - state[5])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[3] = -(sat_pos_vel[0] - state[0])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[4] = -(sat_pos_vel[1] - state[1])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[5] = -(sat_pos_vel[2] - state[2])/sqrt(pow(sat_pos_vel[0] - state[0], 2) + pow(sat_pos_vel[1] - state[1], 2) + pow(sat_pos_vel[2] - state[2], 2));
   out_6914887183114622221[6] = 0;
   out_6914887183114622221[7] = 1;
   out_6914887183114622221[8] = 0;
   out_6914887183114622221[9] = 0;
   out_6914887183114622221[10] = 0;
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

void gnss_update_6(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_6, H_6, NULL, in_z, in_R, in_ea, MAHA_THRESH_6);
}
void gnss_update_20(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_20, H_20, NULL, in_z, in_R, in_ea, MAHA_THRESH_20);
}
void gnss_update_7(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_7, H_7, NULL, in_z, in_R, in_ea, MAHA_THRESH_7);
}
void gnss_update_21(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea) {
  update<1, 3, 0>(in_x, in_P, h_21, H_21, NULL, in_z, in_R, in_ea, MAHA_THRESH_21);
}
void gnss_err_fun(double *nom_x, double *delta_x, double *out_4347860218965336784) {
  err_fun(nom_x, delta_x, out_4347860218965336784);
}
void gnss_inv_err_fun(double *nom_x, double *true_x, double *out_291039236896220829) {
  inv_err_fun(nom_x, true_x, out_291039236896220829);
}
void gnss_H_mod_fun(double *state, double *out_9211819402308797020) {
  H_mod_fun(state, out_9211819402308797020);
}
void gnss_f_fun(double *state, double dt, double *out_4796346524927390369) {
  f_fun(state,  dt, out_4796346524927390369);
}
void gnss_F_fun(double *state, double dt, double *out_4441170918248855648) {
  F_fun(state,  dt, out_4441170918248855648);
}
void gnss_h_6(double *state, double *sat_pos, double *out_3354259565467953634) {
  h_6(state, sat_pos, out_3354259565467953634);
}
void gnss_H_6(double *state, double *sat_pos, double *out_5049104898748510284) {
  H_6(state, sat_pos, out_5049104898748510284);
}
void gnss_h_20(double *state, double *sat_pos, double *out_4970306618701812397) {
  h_20(state, sat_pos, out_4970306618701812397);
}
void gnss_H_20(double *state, double *sat_pos, double *out_3980112008404051449) {
  H_20(state, sat_pos, out_3980112008404051449);
}
void gnss_h_7(double *state, double *sat_pos_vel, double *out_1461985868216609017) {
  h_7(state, sat_pos_vel, out_1461985868216609017);
}
void gnss_H_7(double *state, double *sat_pos_vel, double *out_6914887183114622221) {
  H_7(state, sat_pos_vel, out_6914887183114622221);
}
void gnss_h_21(double *state, double *sat_pos_vel, double *out_1461985868216609017) {
  h_21(state, sat_pos_vel, out_1461985868216609017);
}
void gnss_H_21(double *state, double *sat_pos_vel, double *out_6914887183114622221) {
  H_21(state, sat_pos_vel, out_6914887183114622221);
}
void gnss_predict(double *in_x, double *in_P, double *in_Q, double dt) {
  predict(in_x, in_P, in_Q, dt);
}
}

const EKF gnss = {
  .name = "gnss",
  .kinds = { 6, 20, 7, 21 },
  .feature_kinds = {  },
  .f_fun = gnss_f_fun,
  .F_fun = gnss_F_fun,
  .err_fun = gnss_err_fun,
  .inv_err_fun = gnss_inv_err_fun,
  .H_mod_fun = gnss_H_mod_fun,
  .predict = gnss_predict,
  .hs = {
    { 6, gnss_h_6 },
    { 20, gnss_h_20 },
    { 7, gnss_h_7 },
    { 21, gnss_h_21 },
  },
  .Hs = {
    { 6, gnss_H_6 },
    { 20, gnss_H_20 },
    { 7, gnss_H_7 },
    { 21, gnss_H_21 },
  },
  .updates = {
    { 6, gnss_update_6 },
    { 20, gnss_update_20 },
    { 7, gnss_update_7 },
    { 21, gnss_update_21 },
  },
  .Hes = {
  },
  .sets = {
  },
  .extra_routines = {
  },
};

ekf_init(gnss);
