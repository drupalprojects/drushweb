<?php

class DrushwebHttpClient {
  private $cmd;
  private $oauth_key;
  private $oauth_secret;

  public function __construct($cmd, $oauth_key, $oauth_secret) {
    $this->cmd = $cmd;
    $this->oauth_key = $oauth_key;
    $this->oauth_secret = $oauth_secret;
  }

  /**
   * Send and process HTTP request.
   */
  public function execute($url) {
    $request = new HttpClientRequest($url, array(
      'method' => 'POST',
      'data' => $this->cmd,
      'parameters' => array('q' => 'drushweb/drush/execute'),
    ));
    $formatter = new HttpClientBaseFormatter(HttpClientBaseFormatter::FORMAT_JSON);
    $client = new HttpClient($this->getAuth(), $formatter);
    try {
      return $client->execute($request);
    }
    catch (HttpClientException $e) {
      switch ($e->getCode()) {
        case 301:
        case 302:
        case 303:
        case 305:
        case 307:
          $headers = $this->parseHeaders($e->getResponse()->headers);
          return $this->execute($headers['location']);
        default:
          throw $e;
      }
    }
  }

  /**
   * Get Authentication object.
   *
   * @return
   *   Authentication object.
   */
  protected function getAuth() {
    $consumer = new DrupalOAuthConsumer($this->oauth_key, $this->oauth_secret);
    $sign = DrupalOAuthClient::signatureMethod();
    return new HttpClientOAuth($consumer, NULL, $sign, TRUE, TRUE);
  }


  /**
   * Parse HTTP headers in associative array
   *
   * @todo http_parse_headers (pecl extension).
   * @todo drupalGetHeaders (cf simpletest).
   */
  protected function parseHeaders($content) {
    $headers = array();
    $lines = explode("\n", $content);
    foreach ($lines as $line) {
      list($name, $value) = explode(':', $line, 2);
      if ($value) {
        $headers[strtolower($name)] = trim($value);
      }
    }
    return $headers;
  }

}
