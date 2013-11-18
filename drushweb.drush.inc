<?php

/**
 * Implements hook_drush_init().
 */
function drushweb_server_drush_init() {
  if (drush_get_option('drushweb')) {
    // Set custom log callback if command was sent from webservice client.
    drush_set_context('DRUSH_LOG_CALLBACK', 'drushweb_server_print_log');
    // Set --no by default (we do not support interactive interface).
    if (!drush_get_context('DRUSH_NEGATIVE') && !drush_get_context('DRUSH_AFFIRMATIVE')) {
      drush_set_context('DRUSH_NEGATIVE', TRUE);
    }
  }
}

/**
 * Custom callback for printing output.
 *
 * Encode output with JSON.
 */
function drushweb_server_print_log($entry) {
  $output = json_encode($entry);
  drush_print($output);
  return $output;
}
