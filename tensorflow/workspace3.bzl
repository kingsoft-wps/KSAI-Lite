"""TensorFlow workspace initialization. Consult the WORKSPACE on how to use it."""

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")
load("//third_party:tf_runtime/workspace.bzl", tf_runtime = "repo")

def workspace():
    http_archive(
        name = "io_bazel_rules_closure",
        sha256 = "5b00383d08dd71f28503736db0500b6fb4dda47489ff5fc6bed42557c07c6ba9",
        strip_prefix = "rules_closure-308b05b2419edb5c8ee0471b67a40403df940149",
        urls = [
            "https://storage.googleapis.com/mirror.tensorflow.org/github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",
            "https://github.com/bazelbuild/rules_closure/archive/308b05b2419edb5c8ee0471b67a40403df940149.tar.gz",  # 2019-06-13
        ],
    )

    http_archive(
        name = "tf_toolchains",
        sha256 = "794ec13d2fdbc0a6fd66bcffad63cb967448e93e018151a58bbfac9808883770",
        strip_prefix = "toolchains-1.1.21",
        urls = [
            "http://mirror.tensorflow.org/github.com/tensorflow/toolchains/archive/v1.1.21.tar.gz",
            "https://github.com/tensorflow/toolchains/archive/v1.1.21.tar.gz",
        ],
    )

    tf_runtime()

# Alias so it can be loaded without assigning to a different symbol to prevent
# shadowing previous loads and trigger a buildifier warning.
tf_workspace3 = workspace
