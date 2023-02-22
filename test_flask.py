# # 导入Flask类库
# from flask import Flask
# # 创建应用实例
# app = Flask(__name__)
# # 视图函数（路由）
# @app.route('/')
# def index():
#     return '<h1>Hello Flask!<h1>'
# # 启动实施（只在当前模块运行）
# if __name__ == '__main__':
#     app.run()

from flask import Flask, request
from flask_restful import Resource, Api, abort, reqparse

app = Flask(__name__)
api = Api(app)

class HelloResource(Resource):
    def get(self):
        return { 'message': 'Hello' }

class SayHiResource(Resource):
    def get(self):
        # parser = reqparse.RequestParser()
        # parser.add_argument('name', required=True, help='Name cannot be blank')
        # args = parser.parse_args()
        username = request.args.get("username")
        status = request.args.get("status")

        return { 'message': 'Hello {}, your status is: {}'.format(username, status) }

api.add_resource(HelloResource, '/hello')
api.add_resource(SayHiResource, '/say-hi/<username>')

if __name__ == '__main__':
    app.run(port = 1234, debug=True, load_dotenv=True)

