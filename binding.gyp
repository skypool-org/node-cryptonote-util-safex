{
    "targets": [
        {
            "target_name": "cryptonote",
            "sources": [
                "src/main.cc",
                "src/cryptonote_basic/cryptonote_format_utils.cpp",
                "src/crypto/tree-hash.c",
                "src/crypto/crypto.cpp",
                "src/crypto/crypto-ops.c",
                "src/crypto/crypto-ops-data.c",
                "src/crypto/hash.c",
                "src/crypto/keccak.c",
                "src/crypto/blake256.c",
                "src/crypto/hash-extra-blake.c",
                "src/crypto/groestl.c",
                "src/crypto/hash-extra-groestl.c",
                "src/crypto/jh.c",
                "src/crypto/hash-extra-jh.c",
                "src/crypto/skein.c",
                "src/crypto/hash-extra-skein.c",
                "src/crypto/slow-hash.cpp",
                "src/crypto/oaes_lib.c",
                "src/common/base58.cpp",
                "src/contrib/easylogging++/easylogging++.cc",
                "src/contrib/epee/src/wipeable_string.cpp",
                "src/contrib/epee/src/memwipe.c"
            ],
            "include_dirs": [
                "src",
                "src/cryptonote_protocol",
                "src/cryptonote_core",
                "src/cryptonote_basic",
                "src/ringct",
                "src/contrib/epee/include",
                "src/contrib/easylogging++",
                "<!(node -e \"require('nan')\")",
            ],
            "link_settings": {
                "libraries": [
                    "-lboost_system",
                    "-lboost_date_time",
                ],
            },
            'defines': [
                'AUTO_INITIALIZE_EASYLOGGINGPP',
                'BLOCKCHAIN_DB=DB_LMDB',
                "DEFAULT_DB_TYPE=\"lmdb\"",
                'HAVE_READLINE',
                'HAVE_STRPTIME'

            ],
            'cflags!': ['-O3',],
            'cflags': ["-maes","-march=x86-64"],
            'cflags_cc!': [ '-fno-rtti', '-fno-exceptions', '-std=gnu++0x'],
            "cflags_cc": [
                  "-march=x86-64",
                  "-std=c++11",
                  "-fexceptions",
                  "-frtti",
                  "-fno-strict-aliasing",
                  "-maes",
                  "-D_GNU_SOURCE",
                  "-Wno-unused-variable",
                  "-fstack-protector",
                  "-fstack-protector-strong",
                  "-fno-strict-aliasing",
                  "-O2"
                  
            ],
        }
    ]
}