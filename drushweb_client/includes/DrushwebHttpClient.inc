<?php

class DrushwebHttpClient {
  private $cmd;
  private $response;

  public function __construct($cmd) {
    $this->cmd = $cmd;
    $this->response = array();
  }

  /**
   * Send and process HTTP request.
   */
  public function execute() {
    global $base_url;
    // TODO / -> support HTTP 3XX
    $request = new HttpClientRequest($base_url . '/', array(
      'method' => 'POST',
      'data' => $this->cmd,
      'parameters' => array('q' => 'drushweb/drush/execute'),
    ));
    $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);
    $client = new HttpClient($this->getAuth(), $formatter);
    try {
      $this->response = $client->execute($request);
    }
    catch (HttpClientException $e) {
      throw $e;
    }
  }

  /**
   * Get Authentication object.
   *
   * @return
   *   Authentication object.
   */
  protected function getAuth() {
    return NULL;
  }

  /**
   * Format and print the body of the server response.
   */
  public function printResponse() {
    // TODO execute() might not return an array
    // TODO the response might not be splittable with \n
    foreach ($this->response as $lines) {
      foreach (explode("\n", $lines) as $line) {
        $entry = json_decode($line);
        if (is_object($entry)) {
          drush_log($entry->message, $entry->type, $entry->error);
        }
        else {
          drush_print($line);
        }
      }
    }
  }

}
