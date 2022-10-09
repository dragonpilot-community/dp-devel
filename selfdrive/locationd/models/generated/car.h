#pragma once
#include "rednose/helpers/common_ekf.h"
extern "C" {
void car_update_25(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_24(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_30(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_26(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_27(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_29(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_28(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_update_31(double *in_x, double *in_P, double *in_z, double *in_R, double *in_ea);
void car_err_fun(double *nom_x, double *delta_x, double *out_1074556233241015822);
void car_inv_err_fun(double *nom_x, double *true_x, double *out_2611153403417199822);
void car_H_mod_fun(double *state, double *out_544146871449396538);
void car_f_fun(double *state, double dt, double *out_4702214555172441875);
void car_F_fun(double *state, double dt, double *out_3148325686485299070);
void car_h_25(double *state, double *unused, double *out_1495327568566357005);
void car_H_25(double *state, double *unused, double *out_5153499558744102821);
void car_h_24(double *state, double *unused, double *out_4574568714968761304);
void car_H_24(double *state, double *unused, double *out_2980849959738603255);
void car_h_30(double *state, double *unused, double *out_2945958961195240172);
void car_H_30(double *state, double *unused, double *out_7671832517251351448);
void car_h_26(double *state, double *unused, double *out_4264950525058981339);
void car_H_26(double *state, double *unused, double *out_1411996239870046597);
void car_h_27(double *state, double *unused, double *out_2657132464078969027);
void car_H_27(double *state, double *unused, double *out_8551317485274256951);
void car_h_29(double *state, double *unused, double *out_2055714306465605473);
void car_H_29(double *state, double *unused, double *out_8182063861565743632);
void car_h_28(double *state, double *unused, double *out_8572763217812831363);
void car_H_28(double *state, double *unused, double *out_3099664844496213058);
void car_h_31(double *state, double *unused, double *out_9070143854389145426);
void car_H_31(double *state, double *unused, double *out_785788137636695121);
void car_predict(double *in_x, double *in_P, double *in_Q, double dt);
void car_set_mass(double x);
void car_set_rotational_inertia(double x);
void car_set_center_to_front(double x);
void car_set_center_to_rear(double x);
void car_set_stiffness_front(double x);
void car_set_stiffness_rear(double x);
}