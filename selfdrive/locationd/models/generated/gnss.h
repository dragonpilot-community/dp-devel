#pragma once
#include "rednose/helpers/common_ekf.h"
extern "C" {
void gnss_update_6(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_update_20(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_update_7(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_update_21(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void gnss_err_fun(double *nom_x, double *delta_x, double *out_4347860218965336784);
void gnss_inv_err_fun(double *nom_x, double *true_x, double *out_291039236896220829);
void gnss_H_mod_fun(double *state, double *out_9211819402308797020);
void gnss_f_fun(double *state, double dt, double *out_4796346524927390369);
void gnss_F_fun(double *state, double dt, double *out_4441170918248855648);
void gnss_h_6(double *state, double *sat_pos, double *out_3354259565467953634);
void gnss_H_6(double *state, double *sat_pos, double *out_5049104898748510284);
void gnss_h_20(double *state, double *sat_pos, double *out_4970306618701812397);
void gnss_H_20(double *state, double *sat_pos, double *out_3980112008404051449);
void gnss_h_7(double *state, double *sat_pos_vel, double *out_1461985868216609017);
void gnss_H_7(double *state, double *sat_pos_vel, double *out_6914887183114622221);
void gnss_h_21(double *state, double *sat_pos_vel, double *out_1461985868216609017);
void gnss_H_21(double *state, double *sat_pos_vel, double *out_6914887183114622221);
void gnss_predict(double *in_x, double *in_P, double *in_Q, double dt);
}