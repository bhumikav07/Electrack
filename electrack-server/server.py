import tornado.ioloop
import tornado.web
import tornado.websocket
import json

from prediction import Prediction

pred = Prediction()
    

class EchoWebSocket(tornado.websocket.WebSocketHandler):
    def open(self):
        print("WebSocket opened")

    def on_message(self, message):
        self.write_message(u"You said: " + message)

    def on_close(self):
        print("WebSocket closed")

class MainHandler(tornado.web.RequestHandler):
    def get(self):
        self.write('{"message": "Hello, world"}')


class VisualizeHistoryHandler(tornado.web.RequestHandler):
    def get(self):
        self.render('data/visualize.json')

class ForecastHandler(tornado.web.RequestHandler):
    def get(self):
        self.render('data/forecast.json')

def make_app():
    return tornado.web.Application([
        (r"/", MainHandler),
        (r"/visualize-history", VisualizeHistoryHandler),
        (r"/forecast", ForecastHandler),
        (r"/predict", EchoWebSocket),
    ],debug=True, autoreload=True)

if __name__ == "__main__":
    app = make_app()
    app.listen(8888)
    tornado.ioloop.IOLoop.current().start()