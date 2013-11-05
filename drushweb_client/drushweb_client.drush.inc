<?php
/**
 * @file
 * Drush webservice Client module drush integration.
 */

/**
 * Implements hook_drush_command().
 */
function drushweb_client_drush_command() {
  $items['webservice'] = array(
    'description' => 'Execute a drush command on a site running Drush webservice Server.',
    'arguments' => array(
      'command' => 'A drush command to execute on the server.',
    ),
    'examples' => array(
      'drush webservice enable views' => 'Enable module views on a site.',
    ),
    'bootstrap' => DRUSH_BOOTSTRAP_DRUSH,
    'aliases' => array('web'),
  );
  return $items;
}

/**
 * Implements drush_hook_COMMAND().
 */
function drush_drushweb_client_webservice() {
  $cmd = drush_get_command();
  $cmd = $cmd['arguments'];
  if (drush_get_context('DRUSH_AFFIRMATIVE')) {// TODO support all options ? --verbose etc
    $cmd[] = '--yes';
  }
  $cmd = implode(' ', $cmd);

  try {
    $oauth = new DrushwebHttpClientOAuth($cmd);
    $oauth->execute()->printReponse();
  }
  catch (HttpClientException $e) {
    switch ($e->getCode()) {

      // TODO support 3xx codes

      case 401:
        // TODO make creds storable somewhere
        // TODO include realm 401 ?
        drush_print($e->getMessage());
        // TODO we support only basic http authentication
        // TODO support password occultation
        try {
          $oauth_basic = new DrushwebHttpClientOAuthBasic($cmd);
          $oauth_basic->execute()->printResponse();
        }
        catch (HttpClientException $e2) {
          // TODO cleanly handle $e2 (user abort, 'failed to communicate' message etc)
          drush_print($e2->getMessage());
        }
        break;

      default:
        $msg = dt('Failed to communicate with the server: !msg', array('!msg' => $e->getMessage()));
        return drush_user_abort($msg);
    }
  }
}


// TODO check PHP prerequisites on client (see #1871498) - or let http_client take care or this
// TODO msg if consumer key invalid
// TODO check drush minimum version
// TODO does it work when drushweb_client is disabled ?
