<?php
/**
 * @file
 * drush_webservice_server.features.inc
 */

/**
 * Implements hook_ctools_plugin_api().
 */
function drush_webservice_server_ctools_plugin_api($module = NULL, $api = NULL) {
  if ($module == "services" && $api == "services") {
    return array("version" => "3");
  }
}
