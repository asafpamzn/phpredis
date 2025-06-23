<?php defined('VALKEY_GLIDE_PHP_TESTRUN') or die("Use TestValkeyGlide.php to run tests!\n");

require_once __DIR__ . "/ValkeyGlideBaseTest.php";

/**
 * ValkeyGlide Features Test
 * Tests various constructor options and features for standalone ValkeyGlide client
 */
class ValkeyGlide_Features_Test extends ValkeyGlideBaseTest {

    public function testBasicConstructor() {
        // Test creating ValkeyGlide with basic configuration
        $redis = new ValkeyGlide([
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ]);

        // Verify the connection works with a simple ping
        $this->assertTrue($redis->ping());
        
        // Clean up
        $redis->close();
    }

    public function testConstructorWithSingleAddress() {
        // Test constructor with single address in proper array format
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $redis = new ValkeyGlide($addresses);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithMultipleAddresses() {
        // Test constructor with multiple addresses for failover
        $addresses = [
            ['host' => '127.0.0.1', 'port' => 6379],
            ['host' => '127.0.0.1', 'port' => 6380],
            ['host' => 'localhost', 'port' => 6381]
        ];
        
        $redis = new ValkeyGlide($addresses);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithTlsDisabled() {
        // Test constructor with TLS explicitly disabled
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $redis = new ValkeyGlide($addresses, false); // use_tls = false
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithCredentials() {
        // Test constructor with credentials (if auth is configured)
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $auth = $this->getAuth();
        if ($auth) {
            $credentials = null;
            if (is_array($auth) && count($auth) > 1) {
                $credentials = ['username' => $auth[0], 'password' => $auth[1]];
            } elseif (is_array($auth)) {
                $credentials = ['password' => $auth[0]];
            } else {
                $credentials = ['password' => $auth];
            }
            
            $redis = new ValkeyGlide($addresses, false, $credentials);
            $this->assertTrue($redis->ping());
            $redis->close();
        } else {
            // Test with null credentials when no auth is configured
            $redis = new ValkeyGlide($addresses, false, null);
            $this->assertTrue($redis->ping());
            $redis->close();
        }
    }

    public function testConstructorWithReadFromPrimary() {
        // Test constructor with READ_FROM_PRIMARY strategy
        $addresses = [
            ['host' => '127.0.0.1', 'port' => 6379],
            ['host' => '127.0.0.1', 'port' => 6380],
            ['host' => 'localhost', 'port' => 6381]
        ];
        
        
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithReadFromPreferReplica() {
        // Test constructor with READ_FROM_PREFER_REPLICA strategy
        $addresses = [
            ['host' => '127.0.0.1', 'port' => 6379],
            ['host' => '127.0.0.1', 'port' => 6380],
            ['host' => 'localhost', 'port' => 6381]
        ];
        
        
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PREFER_REPLICA);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithRequestTimeout() {
        // Test constructor with request timeout in milliseconds
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, 5000); // 5 second timeout
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithReconnectStrategy() {
        // Test constructor with reconnection strategy
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $reconnectStrategy = [
            'num_of_retries' => 3,
            'factor' => 2,
            'exponent_base' => 2
        ];
        
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, $reconnectStrategy);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithDatabaseId() {
        // Test constructor with specific database ID
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        // Test with database 0 (default)
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, null, 0);
        $this->assertTrue($redis->ping());
        $redis->close();
        
        // Test with database 1
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, null, 1);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithClientName() {
        // Test constructor with client name identifier
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $clientName = 'test-client-' . uniqid();
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, null, null, $clientName);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithInflightRequestsLimit() {
        // Test constructor with inflight requests limit
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, null, null, null, 100);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithClientAz() {
        // Test constructor with client availability zone
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, null, null, null, null, 'us-east-1a');
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithAdvancedConfig() {
        // Test constructor with advanced configuration
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $advancedConfig = [
            'connection_timeout' => 5000,
            'socket_timeout' => 3000
        ];
        
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, null, null, null, null, null, $advancedConfig);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithLazyConnect() {
        // Test constructor with lazy connection enabled
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, null, null, null, null, null, null, true);
        $this->assertTrue($redis->ping());
        $redis->close();
        
        // Test with lazy connection disabled
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, null, null, null, null, null, null, false);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithAllParameters() {
        // Test constructor with all parameters specified
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $credentials = null;
        $auth = $this->getAuth();
        if ($auth) {
            if (is_array($auth) && count($auth) > 1) {
                $credentials = ['username' => $auth[0], 'password' => $auth[1]];
            } elseif (is_array($auth)) {
                $credentials = ['password' => $auth[0]];
            } else {
                $credentials = ['password' => $auth];
            }
        }
        
        $reconnectStrategy = [
            'num_of_retries' => 2,
            'factor' => 1.5
        ];
        
        $advancedConfig = [
            'connection_timeout' => 4000
        ];
        
        $redis = new ValkeyGlide(
            $addresses,                                    // addresses
            false,                                         // use_tls
            $credentials,                                  // credentials
            ValkeyGlide::READ_FROM_PRIMARY,               // read_from
            3000,                                          // request_timeout
            $reconnectStrategy,                            // reconnect_strategy
            0,                                             // database_id
            'comprehensive-test-client',                   // client_name
            50,                                            // inflight_requests_limit
            'test-az',                                     // client_az
            $advancedConfig,                               // advanced_config
            false                                          // lazy_connect
        );
        
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithAzAffinityReadStrategy() {
        // Test constructor with AZ affinity read strategies
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        // Test READ_FROM_AZ_AFFINITY
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_AZ_AFFINITY);
        $this->assertTrue($redis->ping());
        $redis->close();
        
        // Test READ_FROM_AZ_AFFINITY_REPLICAS_AND_PRIMARY
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_AZ_AFFINITY_REPLICAS_AND_PRIMARY);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithVariousTimeouts() {
        // Test constructor with different timeout values
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        // Test with 1 second timeout
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, 1000);
        $this->assertTrue($redis->ping());
        $redis->close();
        
        // Test with 10 second timeout
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, 10000);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorWithDifferentDatabases() {
        // Test constructor with different database IDs
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        // Test databases 0-3
        for ($db = 0; $db <= 3; $db++) {
            $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, null, $db);
            $this->assertTrue($redis->ping());
            
            // Set and get a value to verify we're in the correct database
            $testKey = "test_db_{$db}_" . uniqid();
            $testValue = "value_for_db_{$db}";
            $redis->set($testKey, $testValue);
            $this->assertEquals($testValue, $redis->get($testKey));
            
            $redis->del($testKey); // Clean up
            $redis->close();
        }
    }

    public function testConstructorWithComplexReconnectStrategies() {
        // Test constructor with various reconnect strategy configurations
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        // Simple retry strategy
        $simpleStrategy = ['num_of_retries' => 5];
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, $simpleStrategy);
        $this->assertTrue($redis->ping());
        $redis->close();
        
        // Complex retry strategy
        $complexStrategy = [
            'num_of_retries' => 3,
            'factor' => 2.0,
            'exponent_base' => 1.5,
            'max_delay' => 10000
        ];
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, null, $complexStrategy);
        $this->assertTrue($redis->ping());
        $redis->close();
    }

    public function testConstructorBasicFunctionality() {
        // Test that constructor creates a working client that can perform basic operations
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        $redis = new ValkeyGlide($addresses, false, null, ValkeyGlide::READ_FROM_PRIMARY, 5000);
        
        // Test ping
        $this->assertTrue($redis->ping());        
        
        // Test basic set/get operations
        $testKey = 'constructor_test_' . uniqid();
        $testValue = 'test_value_' . time();
        
        $this->assertTrue($redis->set($testKey, $testValue));
        $this->assertEquals($testValue, $redis->get($testKey));
        
        // Test key exists
        $this->assertEquals(1, $redis->exists($testKey));
        
        // Clean up
        $this->assertEquals(1, $redis->del($testKey));
        $redis->close();
    }

    public function testConstructorParameterValidation() {
        // Test constructor parameter validation and edge cases
        $addresses = [
            ['host' => $this->getHost(), 'port' => $this->getPort()]
        ];
        
        // Test with minimal parameters (addresses only)
        $redis = new ValkeyGlide($addresses);
        $this->assertTrue($redis->ping());
        $redis->close();
        
        // Test with explicit defaults
        $redis = new ValkeyGlide(
            $addresses,                           // addresses
            false,                               // use_tls (default)
            null,                                // credentials (default)
            ValkeyGlide::READ_FROM_PRIMARY,     // read_from (default)
            null,                                // request_timeout (default)
            null,                                // reconnect_strategy (default)
            null,                                // database_id (default)
            null,                                // client_name (default)
            null,                                // inflight_requests_limit (default)
            null,                                // client_az (default)
            null,                                // advanced_config (default)
            null                                 // lazy_connect (default)
        );
        $this->assertTrue($redis->ping());
        $redis->close();
    }
}
?>
