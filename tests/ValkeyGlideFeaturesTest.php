<?php defined('VALKEY_GLIDE_PHP_TESTRUN') or die("Use TestValkeyGlide.php to run tests!\n");

require_once __DIR__ . "/ValkeyGlideTest.php";

/**
 * ValkeyGlide Features Test
 * Tests various constructor options and features for standalone ValkeyGlide client
 */
class ValkeyGlide_Features_Test extends ValkeyGlide_Test {

    public function testBasicConstructor() {
        // Test creating ValkeyGlide with basic configuration
        $redis = new ValkeyGlide([
            'host' => $this->getHost(),
            'port' => $this->getPort(),
        ]);

        // Verify the connection works with a simple ping
        $this->assertTrue($redis->ping());
        
        // Clean up
        $redis->close();
    }
}
?>
