#############################################################################
# Copyright (c) 2023, Hind Montassif
#
# Distributed under the terms of the BSD 3-Clause License.
#
# The full license is in the file LICENSE, distributed with this software.
#############################################################################

#############################################################################
# TODO fake test to replace with test below when nelson is available
# as a package (for CI)

import unittest

class TestFake(unittest.TestCase):

    def test_fake(self):
        pass

#############################################################################

#import tempfile
#import unittest
#import jupyter_kernel_test

#class KernelTests(jupyter_kernel_test.KernelTests):

    #kernel_name = "xnelson"
    #language_name = "nelson"
    #file_extension = ".nelson"

    #completion_samples = [{"text": "a", "matches": {"addpath"}}]

    ## Samples of code which generate a result value (ie, some text
    ## displayed as Out[n])
    #code_execute_result = [
        #{
            #'code': '6 * 7',
            #'result': 'ans =\n\n    42'
        #}
    #]

    #incomplete_code_samples = ["for("]

    #def test_stderr(self):
        #self.flush_channels()
        #reply, output_msgs = self.execute_helper(code="a = ")
        #self.assertEqual(output_msgs[0]["msg_type"], "error")
        #self.assertEqual(output_msgs[0]["content"]["ename"], "Interpreter error")

#############################################################################

    #code_hello_world = "hello, world"
    #code_page_something = "?"
    #code_inspect_sample = "print"

    #def test_stdout(self):
        #self.flush_channels()
        #reply, output_msgs = self.execute_helper(code="hello, world")
        #self.assertEqual(output_msgs[0]["msg_type"], "stream")
        #self.assertEqual(output_msgs[0]["content"]["name"], "stdout")
        #self.assertEqual(output_msgs[0]["content"]["text"], "hello, world")

if __name__ == "__main__":
    unittest.main()
