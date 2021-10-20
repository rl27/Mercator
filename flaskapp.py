#!/usr/bin/env python

import threading
from flask import Flask, request, make_response, jsonify
from image_sampler.ImageSampler import ImageSampler
import json

#-----------------------------------------------------------------------

app = Flask(__name__)
sampler = ImageSampler()
print("app and sampler created")

#-----------------------------------------------------------------------

@app.route('/get_image', methods=['GET'])
def get_image():

    data = json.loads(request.args.get('data'))
    world_data = data['world']
    set_of_coords = data['coords']
    sampler.generate_images_for_megatile(world_data, set_of_coords)
    
    return jsonify({'message': 'Complete'})