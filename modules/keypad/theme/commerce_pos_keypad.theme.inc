<?php

/**
 * @file
 * Theme callbacks for Commerce POS Cash Input.
 */

/**
 * Implements template_preprocess_commerce_pos_keypad_cash_input_box().
 */
function commerce_pos_keypad_preprocess_commerce_pos_keypad_cash_input_box(&$variables) {
  $variables['inputs'] = array();
  foreach ($variables['denominations'] as $key => $result) {
    $variables['inputs'][$result->denomination_name] = array(
      'title' => $result->denomination_title,
      'amount' => $result->denomination_amount,
    );
  }
}
