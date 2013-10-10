<?php
/**
 * @file
 * drush_remote.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function drush_remote_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "services" && $api == "services") {
    return array("version" => "3");
  }
}
