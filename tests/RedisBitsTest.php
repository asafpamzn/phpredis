<?php defined('PHPREDIS_TESTRUN') or die('Use TestRedis.php to run tests!\n');

require_once __DIR__ . '/TestSuite.php';
require_once __DIR__ . '/SessionHelpers.php';

class Redis_Bits_Test extends TestSuite {
    /**
     * @var Redis
     */
    public $redis;

    /* City lat/long */
    protected $cities = [
        'Chico'         => [-121.837478, 39.728494],
        'Sacramento'    => [-121.494400, 38.581572],
        'Gridley'       => [-121.693583, 39.363777],
        'Marysville'    => [-121.591355, 39.145725],
        'Cupertino'     => [-122.032182, 37.322998]
    ];

    protected $serializers = [
        Redis::SERIALIZER_NONE,
        Redis::SERIALIZER_PHP,
    ];

    protected function getNilValue() {
        return FALSE;
    }

    protected function getSerializers() {
        $result = [Redis::SERIALIZER_NONE, Redis::SERIALIZER_PHP];

        if (defined('Redis::SERIALIZER_IGBINARY'))
            $result[] = Redis::SERIALIZER_IGBINARY;
        if (defined('Redis::SERIALIZER_JSON'))
            $result[] = Redis::SERIALIZER_JSON;
        if (defined('Redis::SERIALIZER_MSGPACK'))
            $result[] = Redis::SERIALIZER_MSGPACK;

        return $result;
    }

    protected function getCompressors() {
        $result['none'] = Redis::COMPRESSION_NONE;
        if (defined('Redis::COMPRESSION_LZF'))
            $result['lzf'] = Redis::COMPRESSION_LZF;
        if (defined('Redis::COMPRESSION_LZ4'))
            $result['lz4'] = Redis::COMPRESSION_LZ4;
        if (defined('Redis::COMPRESSION_ZSTD'))
            $result['zstd'] = Redis::COMPRESSION_ZSTD;

        return $result;
    }

    /* Overridable left/right constants */
    protected function getLeftConstant() {
        return Redis::LEFT;
    }

    protected function getRightConstant() {
        return Redis::RIGHT;
    }

    protected function detectKeyDB(array $info) {
        return strpos($info['executable'] ?? '', 'keydb') !== false ||
               isset($info['keydb']) ||
               isset($info['mvcc_depth']);
    }

    protected function detectValkey(array $info) {
        return isset($info['server_name']) && $info['server_name'] === 'valkey';
    }

    public function setUp() {
        $this->redis = $this->newInstance();
        $info = $this->redis->info();
        $this->version = (isset($info['redis_version'])?$info['redis_version']:'0.0.0');
        $this->is_keydb = $this->detectKeyDB($info);
        $this->is_valkey = $this->detectValKey($info);
    }

    protected function minVersionCheck($version) {
        return version_compare($this->version, $version) >= 0;
    }

    protected function mstime() {
        return round(microtime(true)*1000);
    }

    protected function getAuthParts(&$user, &$pass) {
        $user = $pass = NULL;

        $auth = $this->getAuth();
        if ( ! $auth)
            return;

        if (is_array($auth)) {
            if (count($auth) > 1) {
                list($user, $pass) = $auth;
            } else {
                $pass = $auth[0];
            }
        } else {
            $pass = $auth;
        }
    }

    protected function sessionPrefix(): string {
        return 'PHPREDIS_SESSION:';
    }

    protected function sessionSaveHandler(): string {
        return 'redis';
    }

    protected function sessionSavePath(): string {
        return sprintf('tcp://%s:%d?%s', $this->getHost(), $this->getPort(),
                       $this->getAuthFragment());
    }

    protected function getAuthFragment() {
        $this->getAuthParts($user, $pass);

        if ($user && $pass) {
            return sprintf('auth[user]=%s&auth[pass]=%s', $user, $pass);
        } else if ($pass) {
            return sprintf('auth[pass]=%s', $pass);
        } else {
            return '';
        }
    }

    protected function newInstance() {
        $r = new Redis([
            'host' => $this->getHost(),
            'port' => $this->getPort(),
        ]);

        if ($this->getAuth()) {
            $this->assertTrue($r->auth($this->getAuth()));
        }
        return $r;
    }

    public function tearDown() {
        if ($this->redis) {
            $this->redis->close();
        }
    }

    public function reset() {
        $this->setUp();
        $this->tearDown();
    }

    /* Helper function to determine if the class has pipeline support */
    protected function havePipeline() {
        return false;//TODO pipeline
       // return defined(get_class($this->redis) . '::PIPELINE');
    }

    protected function haveMulti() {
        return defined(get_class($this->redis) . '::MULTI');
    }

    public function testMinimumVersion() {
        $this->assertTrue(version_compare($this->version, '2.4.0') >= 0);
    }

    /* These test cases were generated randomly.  We're just trying to test
       that PhpRedis handles all combination of arguments correctly. */
    public function testBitcount() {
        /* key */
        print "testBitcount !!!!!!!!!!!!!!";
        $this->redis->set('bitcountkey', hex2bin('bd906b854ca76cae'));
        $this->assertEquals(33, $this->redis->bitcount('bitcountkey'));

        /* key, start */
        $this->redis->set('bitcountkey', hex2bin('400aac171382a29bebaab554f178'));
        $this->assertEquals(4, $this->redis->bitcount('bitcountkey', 13));

        /* key, start, end */
        $this->redis->set('bitcountkey', hex2bin('b1f32405'));
        $this->assertEquals(2, $this->redis->bitcount('bitcountkey', 3, 3));

        /* key, start, end BYTE */
        $this->redis->set('bitcountkey', hex2bin('10eb8939e68bfdb640260f0629f3'));
        $this->assertEquals(1, $this->redis->bitcount('bitcountkey', 8, 8, false));

        if ( ! $this->is_keydb && $this->minVersionCheck('7.0')) {
            /* key, start, end, BIT */
            $this->redis->set('bitcountkey', hex2bin('cd0e4c80f9e4590d888a10'));
            $this->assertEquals(5, $this->redis->bitcount('bitcountkey', 0, 9, true));
        }
    }

    public function testBitop() {
        if ( ! $this->minVersionCheck('2.6.0'))
            $this->markTestSkipped();

        $this->redis->set('{key}1', 'foobar');
        $this->redis->set('{key}2', 'abcdef');

        // Regression test for GitHub issue #2210
        $this->assertEquals(6, $this->redis->bitop('AND', '{key}1', '{key}2'));

        // Make sure RedisCluster doesn't even send the command.  We don't care
        // about what Redis returns
        @$this->redis->bitop('AND', 'key1', 'key2', 'key3');
        $this->assertNull($this->redis->getLastError());

        $this->redis->del('{key}1', '{key}2');
    }

    public function testBitsets() {
        $this->redis->del('key');
        $this->assertEquals(0, $this->redis->getBit('key', 0));
        $this->assertFalse($this->redis->getBit('key', -1));
        $this->assertEquals(0, $this->redis->getBit('key', 100000));

        $this->redis->set('key', "\xff");
        for ($i = 0; $i < 8; $i++) {
            $this->assertEquals(1, $this->redis->getBit('key', $i));
        }
        $this->assertEquals(0, $this->redis->getBit('key', 8));

        // change bit 0
        $this->assertEquals(1, $this->redis->setBit('key', 0, 0));
        $this->assertEquals(0, $this->redis->setBit('key', 0, 0));
        $this->assertEquals(0, $this->redis->getBit('key', 0));
        $this->assertKeyEquals("\x7f", 'key');

        // change bit 1
        $this->assertEquals(1, $this->redis->setBit('key', 1, 0));
        $this->assertEquals(0, $this->redis->setBit('key', 1, 0));
        $this->assertEquals(0, $this->redis->getBit('key', 1));
        $this->assertKeyEquals("\x3f", 'key');

        // change bit > 1
        $this->assertEquals(1, $this->redis->setBit('key', 2, 0));
        $this->assertEquals(0, $this->redis->setBit('key', 2, 0));
        $this->assertEquals(0, $this->redis->getBit('key', 2));
        $this->assertKeyEquals("\x1f", 'key');

        // values above 1 are changed to 1 but don't overflow on bits to the right.
        $this->assertEquals(0, $this->redis->setBit('key', 0, 0xff));
        $this->assertKeyEquals("\x9f", 'key');

        // Verify valid offset ranges
        $this->assertFalse($this->redis->getBit('key', -1));

        $this->redis->setBit('key', 0x7fffffff, 1);
        $this->assertEquals(1, $this->redis->getBit('key', 0x7fffffff));
    }

  

    public function testBitPos() {
        if (version_compare($this->version, '2.8.7') < 0) {
            $this->MarkTestSkipped();
            return;
        }

        $this->redis->del('bpkey');

        $this->redis->set('bpkey', "\xff\xf0\x00");
        $this->assertEquals(12, $this->redis->bitpos('bpkey', 0));

        $this->redis->set('bpkey', "\x00\xff\xf0");
        $this->assertEquals(8, $this->redis->bitpos('bpkey', 1, 0));
        $this->assertEquals(8, $this->redis->bitpos('bpkey', 1, 1));

        $this->redis->set('bpkey', "\x00\x00\x00");
        $this->assertEquals(-1, $this->redis->bitpos('bpkey', 1));

        if ( ! $this->minVersionCheck('7.0.0'))
            return;

        $this->redis->set('bpkey', "\xF");
        $this->assertEquals(4, $this->redis->bitpos('bpkey', 1, 0, -1, true));
        $this->assertEquals(-1,  $this->redis->bitpos('bpkey', 1, 1, -1));
        $this->assertEquals(-1,  $this->redis->bitpos('bpkey', 1, 1, -1, false));
    }

  
    public function testXRead() {
        if ( ! $this->minVersionCheck('5.0'))
            $this->markTestSkipped();

        foreach ($this->getSerializers() as $serializer) {
            $this->redis->setOption(Redis::OPT_SERIALIZER, $serializer);
            $this->doXReadTest();
        }

        /* Don't need to test BLOCK multiple times */
        $m1 = round(microtime(true)*1000);
        $this->redis->xRead(['somestream' => '$'], -1, 100);
        $m2 = round(microtime(true)*1000);
        $this->assertGT(99, $m2 - $m1);
    }

    protected function compareStreamIds($redis, $control) {
        foreach ($control as $stream => $ids) {
            $rcount = count($redis[$stream]);
            $lcount = count($control[$stream]);

            /* We should have the same number of messages */
            $this->assertEquals($rcount, $lcount);

            /* We should have the exact same IDs */
            foreach ($ids as $k => $id) {
                $this->assertTrue(isset($redis[$stream][$id]));
            }
        }
    }

    public function testXReadGroup() {
        if ( ! $this->minVersionCheck('5.0'))
            $this->markTestSkipped();

        /* Create some streams and groups */
        $streams = ['{s}-1', '{s}-2'];
        $groups = ['group1' => 0, 'group2' => 0];

        /* I'm not totally sure why Redis behaves this way, but we have to
         * send '>' first and then send ID '0' for subsequent xReadGroup calls
         * or Redis will not return any messages.  This behavior changed from
         * redis 5.0.1 and 5.0.2 but doing it this way works for both versions. */
        $qcount = 0;
        $query1 = ['{s}-1' => '>', '{s}-2' => '>'];
        $query2 = ['{s}-1' => '0', '{s}-2' => '0'];

        $ids = $this->addStreamsAndGroups($streams, 1, $groups);

        /* Test that we get get the IDs we should */
        foreach (['group1', 'group2'] as $group) {
            foreach ($ids as $stream => $messages) {
                while ($ids[$stream]) {
                    /* Read more messages */
                    $query = !$qcount++ ? $query1 : $query2;
                    $resp = $this->redis->xReadGroup($group, 'consumer', $query);

                    /* They should match with our local control array */
                    $this->compareStreamIds($resp, $ids);

                    /* Remove a message from our control *and* XACK it in Redis */
                    $id = array_shift($ids[$stream]);
                    $this->redis->xAck($stream, $group, [$id]);
                }
            }
        }

        /* Test COUNT option */
        for ($c = 1; $c <= 3; $c++) {
            $this->addStreamsAndGroups($streams, 3, $groups);
            $resp = $this->redis->xReadGroup('group1', 'consumer', $query1, $c);

            foreach ($resp as $stream => $smsg) {
                $this->assertEquals(count($smsg), $c);
            }
        }

        /* Test COUNT option with NULL (should be ignored) */
        $this->addStreamsAndGroups($streams, 3, $groups, NULL);
        $resp = $this->redis->xReadGroup('group1', 'consumer', $query1, NULL);
        foreach ($resp as $stream => $smsg) {
            $this->assertEquals(count($smsg), 3);
        }

        /* Finally test BLOCK with a sloppy timing test */
        $tm1 = $this->mstime();
        $qnew = ['{s}-1' => '>', '{s}-2' => '>'];
        $this->redis->xReadGroup('group1', 'c1', $qnew, 0, 100);
        $this->assertGTE(100, $this->mstime() - $tm1);

        /* Make sure passing NULL to block doesn't block */
        $tm1 = $this->mstime();
        $this->redis->xReadGroup('group1', 'c1', $qnew, NULL, NULL);
        $this->assertLT(100, $this->mstime() - $tm1);

        /* Make sure passing bad values to BLOCK or COUNT immediately fails */
        $this->assertFalse(@$this->redis->xReadGroup('group1', 'c1', $qnew, -1));
        $this->assertFalse(@$this->redis->xReadGroup('group1', 'c1', $qnew, NULL, -1));
    }

    public function testXPending() {
        if ( ! $this->minVersionCheck('5.0'))
            $this->markTestSkipped();

        $rows = 5;
        $this->addStreamsAndGroups(['s'], $rows, ['group' => 0]);

        $msg = $this->redis->xReadGroup('group', 'consumer', ['s' => 0]);
        $ids = array_keys($msg['s']);

        for ($n = count($ids); $n >= 0; $n--) {
            $xp = $this->redis->xPending('s', 'group');

            $this->assertEquals(count($ids), $xp[0]);

            /* Verify we're seeing the IDs themselves */
            for ($idx = 1; $idx <= 2; $idx++) {
                if ($xp[$idx]) {
                    $this->assertPatternMatch('/^[0-9].*-[0-9].*/', $xp[$idx]);
                }
            }

            if ($ids) {
                $id = array_shift($ids);
                $this->redis->xAck('s', 'group', [$id]);
            }
        }

        /* Ensure we can have NULL trailing arguments */
        $this->assertTrue(is_array($this->redis->xpending('s', 'group', '-', '+', 1, null)));
        $this->assertTrue(is_array($this->redis->xpending('s', 'group', NULL, NULL, -1, NULL)));
    }

   
}
?>
