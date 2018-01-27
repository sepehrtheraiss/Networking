#!/usr/bin/env python3

# if a library is missing, you can install it with 'pip install <lib> --user'
import os
# library for controlling subprocesses
from pexpect import spawn, run
# library for unit testing
import unittest as ut
# library for temporary files/directories
import tempfile as tf

port = "1238"
ip = "127.0.0.1"
server_name = os.path.abspath(os.path.join(os.getcwd(),
                                           "ex1/bin/remotesrv"))
client_name = os.path.abspath(os.path.join(os.getcwd(),
                                           "ex1/bin/remotecli"))
prompt = "> "

class TestClient(ut.TestCase):
    # called before each test
    def setUp(self):
        self.wdir = tf.TemporaryDirectory()
        # creates a subprocess for the server
        # cwd changes the work directory for the process
        self.server = spawn(server_name + " " + port, cwd=self.wdir.name)
        # creates the client subprocess
        # echo true means that the client repeats the input in the output
        self.client = spawn(" ".join([client_name, ip, port]), echo=False)
        # populates the temporary directory
        run("touch file1 test2 extra Extra", cwd=self.wdir.name)

    # called after each test
    def tearDown(self):
        try:
            self.wdir.cleanup()
            self.server.terminate()
            self.client.terminate()
        except:
            self.assertFail()

    # all methods started with "test_" are tests and will be called in the main
    def test_ls(self):
        # expect to see the prompt
        # if using the prompt from the assignment manual, escape the $
        # 'client \$ '
        # otherwise $ will be interpreted as end of line and the expect will fail
        self.client.expect(prompt)
        self.client.sendline("ls")
        self.client.expect(prompt)
        # the variable before holds all output that appears between expects
        result = self.client.before
        expected = run("ls", cwd=self.wdir.name)
        # makes sure that all files in the client's output are in the 'ls' output
        self.assertEqual(result.split(), expected.split())

if __name__ == "__main__":
    # this runs the 'test_*' methods of the 'TestCase' classes
    ut.main()