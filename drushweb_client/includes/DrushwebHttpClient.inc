<?php

class DrushwebHttpClient {
  private $cmd;
  private $consumer_key;
  private $consumer_secret;

  public function __construct($cmd) {
    $this->cmd = $cmd;
    $this->response = array();
    // TODO make this dynamic
    $this->consumer_key = 'y4ZYMrw8VNMDsnkibMxwu8ZybS4GPzHV';
    $this->consumer_secret = 'JHjnXtfpLxrPCVwnjyn8dCQmsxNmgfEZ';
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
    $consumer = new DrupalOAuthConsumer($this->consumer_key, $this->consumer_secret);
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
