<?php

class HttpClientOAuthBasic extends HttpClientOAuth {
  private $http_user;
  private $http_pass;

  public function __construct($http_user, $http_pass, $consumer, $token = NULL, $sign_impl = NULL, $hash_body = TRUE, $header_auth = FALSE, $realm = NULL) {
    parent::__construct($consumer, $token, $sign_impl, $hash_body, $header_auth, $realm);
    $this->http_user = $http_user;
    $this->http_pass = $http_pass;
  }

  /**
   * @override
   */
  public function authenticate($request) {
    parent::authenticate($request);
    $request->setHeader('Authorization', array('Basic ' . base64_encode("$this->http_user:$this->http_pass")));
  }
}
