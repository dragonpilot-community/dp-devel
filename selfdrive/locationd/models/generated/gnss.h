#pragma once
#include "rednose/helpers/common_ekf.h"
extern "C" {
void gnss_update_6(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_update_20(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_update_7(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_update_21(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_err_fun(double *nom_x, double *delta_x, double *out_794957986474022410);
void gnss_inv_err_fun(double *nom_x, double *true_x, double *out_7380037827036718422);
void gnss_H_mod_fun(double *state, double *out_3537326562386189174);
void gnss_f_fun(double *state, double dt, double *out_3201302353069732401);
void gnss_F_fun(double *state, double dt, double *out_4949750205820914524);
void gnss_h_6(double *state, double *sat_pos, double *out_4222654111953870605);
void gnss_H_6(double *state, double *sat_pos, double *out_1907527026073269766);
void gnss_h_20(double *state, double *sat_pos, double *out_7050938256011114664);
void gnss_H_20(double *state, double *sat_pos, double *out_6133602518692230455);
void gnss_h_7(double *state, double *sat_pos_vel, double *out_6212552211602838251);
void gnss_H_7(double *state, double *sat_pos_vel, double *out_2598800062447436765);
void gnss_h_21(double *state, double *sat_pos_vel, double *out_6212552211602838251);
void gnss_H_21(double *state, double *sat_pos_vel, double *out_2598800062447436765);
void gnss_predict(double *in_x, double *in_P, double *in_Q, double dt);
}