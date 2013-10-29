<?php

/**
 * Implements hook_drush_init().
 *
 * Set custom log callback if command was sent remotely.
 */
function drush_remote_server_drush_init() {
  if (drush_get_option('drush-remote')) {
    drush_set_context('DRUSH_LOG_CALLBACK', '_drush_remote_server_print_log');
  }
}

/**
 * Custom callback for output printing.
 */
function _drush_remote_server_print_log($entry) {
  $output = json_encode($entry);
  drush_print($output);
  return $output;
}
