<?php

/**
 * Povides 2 levels of authentication:
 * - HTTP (Basic)
 * - OAuth
 */
class HttpClientOAuthBasic extends HttpClientOAuth {

  /**
   * @override.
   */
  public function authenticate($request) {
    parent::authenticate($request);
  }

}
