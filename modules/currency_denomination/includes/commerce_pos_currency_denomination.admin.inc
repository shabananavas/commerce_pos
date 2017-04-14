<?php

/**
 * @file
 * Admin-related callbacks and functions for commerce_pos_currency_denomination.
 */

/**
 * The currency denomination admin settings form.
 */
function commerce_pos_currency_denomination_settings_form($form, &$form_state) {
  // Add our add denomination type button.
  $form['commerce_pos_add_denomination_type_button'] = array(
    '#markup' => l(t('Add New Denomination Type'), 'admin/commerce/config/pos/denomination/add', array(
      'attributes' => array('class' => array('button')),
    ))
  );

  $denomination_options = array();
  $results = commerce_pos_denomination_get_currency_types();
  // Format the denomination options for our form element.
  foreach ($results as $result) {
    $markup = $result->currency_code;
    if ($result->currency_code != 'USD') {
      $markup .= ' | ' . l(t('Delete Denomination'), 'admin/commerce/config/pos/denomination-type/' . $result->currency_id . '/delete', array(
        'attributes' => array('class' => array('delete-denomination'))
      ));
    }
    $denomination_options[$result->currency_code] = $markup;
  }

  $form['commerce_pos_default_currency_denomination_types'] = array(
    '#title' => t('Set Default Currency Denominations'),
    '#type' => 'checkboxes',
    '#options' => $denomination_options,
    '#required' => TRUE,
    '#default_value' => variable_get('commerce_pos_default_currency_denomination_types', array('USD')),
    '#description' => t('Select the currency denomination types that will be used in the system.'),
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save settings'),
  );

  return $form;
}

/**
 * Submit handler for the POS currency denomination settings form.
 */
function commerce_pos_currency_denomination_settings_form_submit($form, &$form_state) {
  if (!empty($form_state['values']['commerce_pos_default_currency_denomination_types'])) {
    variable_set('commerce_pos_default_currency_denomination_types', $form_state['values']['commerce_pos_default_currency_denomination_types']);
  }
}

/**
 * The form for adding new denomination types.
 */
function commerce_pos_currency_add_denomination_type_form($form, &$form_state) {
  $form['commerce_pos_denomination_type_currency_code'] = array(
    '#type' => 'textfield',
    '#size' => 3,
    '#maxlength' => 3,
    '#default_value' => NULL,
    '#title' => t('Denomination Type Currency Code'),
    '#description' => t('Currency code should be in the following format: "USD".'),
    '#required' => TRUE,
  );

  $form['actions']['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Save'),
  );

  return $form;
}

/**
 * Validate handler for adding new denomination types.
 */
function commerce_pos_currency_add_denomination_type_form_validate($form, &$form_state) {
  // Check if the denomination type already exists in the db.
  $result = db_select('commerce_pos_currency_denomination_type', 't')
    ->fields('t', array('currency_id'))
    ->condition('currency_code', $form_state['values']['commerce_pos_denomination_type_currency_code'])
    ->execute()
    ->fetchField();

  if ($result) {
    form_set_error('', t('This denomination type already exists in the system.'));
  }
}

/**
 * Submit handler for adding new denomination types.
 */
function commerce_pos_currency_add_denomination_type_form_submit($form, &$form_state) {
  db_insert('commerce_pos_currency_denomination_type')
    ->fields(array(
      'currency_code' => $form_state['values']['commerce_pos_denomination_type_currency_code']
    ))
    ->execute();

  drupal_set_message(t('The denomination type has been successfully added.'));
  drupal_goto('admin/commerce/config/pos/denomination');
}

/**
 * Delete a denomination type.
 */
function commerce_pos_currency_delete_denomination_type_form($form, &$form_state, $currency_id) {
  $form_state['currency_id'] = $currency_id;

  return confirm_form(
    $form,
    t('Are you sure you want to delete this currency denomination type?'),
    'admin/commerce/config/pos/denomination',
    t('All denominations under this type will be deleted. This action cannot be undone.'),
    t('Delete'),
    t('Cancel')
  );

  return $form;
}

/**
 * Submit handler for deleting a denomination type.
 */
function commerce_pos_currency_delete_denomination_type_form_submit($form, &$form_state) {
  if (isset($form_state['currency_id'])) {
    // First delete the denominations for that type.
    db_delete('commerce_pos_currency_denomination')
      ->condition('currency_id', $form_state['currency_id'])
      ->execute();

    // Now delete the denomination type.
    db_delete('commerce_pos_currency_denomination_type')
      ->condition('currency_id', $form_state['currency_id'])
      ->execute();
  }

  drupal_set_message(t('The denomination type has been successfully deleted.'));
  drupal_goto('admin/commerce/config/pos/denomination');
}
