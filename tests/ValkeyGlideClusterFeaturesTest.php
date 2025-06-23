<?php defined('VALKEY_GLIDE_PHP_TESTRUN') or die("Use TestValkeyGlide.php to run tests!\n");

require_once __DIR__ . "/ValkeyGlideClusterTest.php";

/**
 * ValkeyGlide Cluster Features Test
 * Tests various constructor options and features for ValkeyGlideCluster client
 */  
class ValkeyGlide_Cluster_Features_Test extends ValkeyGlide_Cluster_Test {

    public function testBasicClusterConstructor() {
        // Test creating ValkeyGlideCluster with basic configuration
        $redis = new ValkeyGlideCluster(
            [['host' => '127.0.0.1', 'port' => 7001]], // addresses array format
            false, // use_tls
            $this->getAuth(), // credentials
            ValkeyGlide::READ_FROM_PRIMARY // read_from
        );

        // Verify the connection works with a simple ping
        $this->assertTrue($redis->ping(['type' => 'primarySlotKey', 'key' => 'test']));
        
        // Clean up
        $redis->close();
    }

    public function testReadFromReplica() {
        // Test creating ValkeyGlideCluster with read-from-replica configuration
        $redis = new ValkeyGlideCluster(
            [['host' => '127.0.0.1', 'port' => 7001]], // addresses array format
            false, // use_tls
            $this->getAuth(), // credentials
            ValkeyGlide::READ_FROM_REPLICA // read_from replica
        );

        // Verify the connection works with a simple ping
        $this->assertTrue($redis->ping(['type' => 'primarySlotKey', 'key' => 'test']));
        
        // Clean up
        $redis->close();
    }
}
?>
