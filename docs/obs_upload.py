# -*- coding: utf-8 -*-
import argparse
import logging
import os
import time

from obs import ObsClient


logging.basicConfig(level=logging.INFO)


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument('-ak', help='OBS Access key', required=True)
    parser.add_argument('-sk', help='OBS Secret key', required=True)
    parser.add_argument('-bucket', help='OBS bucket name', required=True)
    parser.add_argument('-src', help='upload source path', required=True)
    parser.add_argument('-key', help='upload object key', required=True)
    parser.add_argument('-server', help='OBS upload url', required=True)
    args = parser.parse_args()
    obs_client = ObsClient(access_key_id=args.ak, secret_access_key=args.sk,
                           server=args.server)
    start_time = time.time()
    for root, _, files in os.walk(args.src):
        logging.info('upload OBS, path: %s, files: %s', root, len(files))
        for fn in files:
            real_path = os.path.join(root, fn)
            rel_path = os.path.relpath(real_path, start=args.src)
            obj_key = os.path.join(args.key, rel_path)
            response = obs_client.putFile(args.bucket, obj_key, real_path)
            if not isinstance(response, dict) or response.get('status') != 200:
                logging.error('---> upload failed, %s', response)
                obs_client.close()
        logging.info('---> upload success!')
    end_time = time.time()
    logging.info('upload take seconds: %s', end_time - start_time)
    obs_client.close()


if __name__ == '__main__':
    main()
