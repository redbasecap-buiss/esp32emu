#include "esp_http_client.h"
#include <cassert>
#include <cstdio>
#include <cstring>

static int event_count = 0;
static bool got_data = false;

static esp_err_t test_event_handler(esp_http_client_event_t* evt) {
    event_count++;
    if (evt->event_id == HTTP_EVENT_ON_DATA && evt->data_len > 0) {
        got_data = true;
    }
    return ESP_OK;
}

int main() {
    // Test basic init/perform/cleanup
    {
        esp_http_client_config_t config{};
        config.url = "http://example.com/api";
        config.method = HTTP_METHOD_GET;

        auto client = esp_http_client_init(&config);
        assert(client != nullptr);

        assert(esp_http_client_perform(client) == ESP_OK);
        assert(esp_http_client_get_status_code(client) == 200);
        assert(esp_http_client_get_content_length(client) > 0);

        // Read response
        char buf[256] = {};
        int read = esp_http_client_read(client, buf, sizeof(buf));
        assert(read > 0);
        assert(strstr(buf, "ok") != nullptr);

        assert(esp_http_client_cleanup(client) == ESP_OK);
    }

    // Test POST with headers
    {
        esp_http_client_config_t config{};
        config.url = "http://example.com/post";
        config.method = HTTP_METHOD_POST;

        auto client = esp_http_client_init(&config);
        assert(client != nullptr);

        esp_http_client_set_header(client, "Content-Type", "application/json");
        const char* body = "{\"key\":\"value\"}";
        esp_http_client_set_post_field(client, body, (int)strlen(body));

        assert(esp_http_client_perform(client) == ESP_OK);
        assert(esp_http_client_get_status_code(client) == 200);

        esp_http_client_cleanup(client);
    }

    // Test event handler
    {
        event_count = 0;
        got_data = false;

        esp_http_client_config_t config{};
        config.url = "http://example.com/events";
        config.event_handler = test_event_handler;

        auto client = esp_http_client_init(&config);
        esp_http_client_perform(client);

        assert(event_count >= 3); // connected, data, finish
        assert(got_data == true);

        esp_http_client_cleanup(client);
    }

    // Test set_url and set_method
    {
        esp_http_client_config_t config{};
        config.url = "http://original.com";

        auto client = esp_http_client_init(&config);
        esp_http_client_set_url(client, "http://changed.com/new");
        esp_http_client_set_method(client, HTTP_METHOD_PUT);

        assert(client->url == "http://changed.com/new");
        assert(client->method == HTTP_METHOD_PUT);

        esp_http_client_cleanup(client);
    }

    // Test inject custom response
    {
        esp_http_client_config_t config{};
        config.url = "http://example.com/custom";

        auto client = esp_http_client_init(&config);
        client->response_body = "custom_response_data";
        client->status_code = 201;
        client->performed = true; // prevent default override

        esp_http_client_perform(client);
        // When performed=true before calling, it keeps the injected values
        assert(esp_http_client_get_status_code(client) == 201);

        char buf[64] = {};
        int n = esp_http_client_read(client, buf, sizeof(buf));
        assert(n == 20);
        assert(strcmp(buf, "custom_response_data") == 0);

        esp_http_client_cleanup(client);
    }

    // Test null safety
    assert(esp_http_client_init(nullptr) == nullptr);
    assert(esp_http_client_set_url(nullptr, "x") == ESP_ERR_INVALID_ARG);
    assert(esp_http_client_get_status_code(nullptr) == -1);
    assert(esp_http_client_cleanup(nullptr) == ESP_OK);

    printf("test_esp_http_client: all assertions passed\n");
    return 0;
}
