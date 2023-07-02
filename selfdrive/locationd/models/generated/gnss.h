#pragma once
#include "rednose/helpers/common_ekf.h"
extern "C" {
void gnss_update_6(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_update_20(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_update_7(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_update_21(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_err_fun(double *nom_x, double *delta_x, double *out_4569436310324962612);
void gnss_inv_err_fun(double *nom_x, double *true_x, double *out_6154689448416067841);
void gnss_H_mod_fun(double *state, double *out_2111331473325142948);
void gnss_f_fun(double *state, double dt, double *out_123904438329706631);
void gnss_F_fun(double *state, double dt, double *out_2058084259701846888);
void gnss_h_6(double *state, double *sat_pos, double *out_987596647970336462);
void gnss_H_6(double *state, double *sat_pos, double *out_7468333872343083007);
void gnss_h_20(double *state, double *sat_pos, double *out_4625491446440568630);
void gnss_H_20(double *state, double *sat_pos, double *out_5841209689797279753);
void gnss_h_7(double *state, double *sat_pos_vel, double *out_2359062028120463062);
void gnss_H_7(double *state, double *sat_pos_vel, double *out_3274184746209643999);
void gnss_h_21(double *state, double *sat_pos_vel, double *out_2359062028120463062);
void gnss_H_21(double *state, double *sat_pos_vel, double *out_3274184746209643999);
void gnss_predict(double *in_x, double *in_P, double *in_Q, double dt);
}