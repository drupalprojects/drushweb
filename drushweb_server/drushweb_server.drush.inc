<?php

/**
 * Implements hook_drush_init().
 *
 * Set custom log callback if command was sent from webservice client.
 */
function drushweb_server_drush_init() {
  if (drush_get_option('drushweb')) {
    drush_set_context('DRUSH_LOG_CALLBACK', '_drushweb_server_print_log');
    // Set --no by default (we do not support interactive interface).
    if (!drush_get_context('DRUSH_NEGATIVE') && !drush_get_context('DRUSH_AFFIRMATIVE')) {
      drush_set_context('DRUSH_NEGATIVE', TRUE);
    }
  }
}

/**
 * Custom callback for output printing.
 */
function _drushweb_server_print_log($entry) {
  $output = json_encode($entry);
  drush_print($output);
  return $output;
}
