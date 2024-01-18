/**
 * @brief The maximum duration between non-empty network reads while receiving
 * an HTTP response via the #HTTPClient_Send API function.
 *
 * The transport receive function may be called multiple times until the end of
 * the response is detected by the parser. This timeout represents the maximum
 * duration that is allowed without any data reception from the network for the
 * incoming response.
 *
 * If the timeout expires, the #HTTPClient_Send function will return
 * #HTTPNetworkError.
 *
 * If #HTTPResponse_t.getTime is set to NULL, then this HTTP_RECV_RETRY_TIMEOUT_MS
 * is unused. When this timeout is unused, #HTTPClient_Send will not retry the
 * transport receive calls that return zero bytes read.
 *
 * <b>Possible values:</b> Any positive 32 bit integer. A small timeout value
 * is recommended. <br>
 * <b>Default value:</b> `10`
 */
#define HTTP_RECV_RETRY_TIMEOUT_MS    ( 1000U )

/**
 * @brief The maximum duration between non-empty network transmissions while
 * sending an HTTP request via the #HTTPClient_Send API function.
 *
 * When sending an HTTP request, the transport send function may be called multiple
 * times until all of the required number of bytes are sent.
 * This timeout represents the maximum duration that is allowed for no data
 * transmission over the network through the transport send function.
 *
 * If the timeout expires, the #HTTPClient_Send function returns #HTTPNetworkError.
 *
 * If #HTTPResponse_t.getTime is set to NULL, then this HTTP_RECV_RETRY_TIMEOUT_MS
 * is unused. When this timeout is unused, #HTTPClient_Send will not retry the
 * transport send calls that return zero bytes sent.
 *
 * <b>Possible values:</b> Any positive 32 bit integer. A small timeout value
 * is recommended. <br>
 * <b>Default value:</b> `10`
 */
#define HTTP_SEND_RETRY_TIMEOUT_MS    ( 1000U )
