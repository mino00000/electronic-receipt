from flask import Flask, request, jsonify
from flask_sqlalchemy import SQLAlchemy
import os

app = Flask(__name__)

# Database setup
app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///pos.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = False
db = SQLAlchemy(app)

# Define a payment model
class Payment(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    email = db.Column(db.String(120), nullable=False)
    amount = db.Column(db.Float, nullable=False)
    date = db.Column(db.String(100), nullable=False)

    def __repr__(self):
        return f"<Payment {self.email}, {self.amount}>"

# Create the database
@app.before_first_request
def create_tables():
    db.create_all()

# Endpoint to receive payment data
@app.route('/payment', methods=['POST'])
def payment_page():
    return '''
    <form action="/payment" method="post">
        <input type="text" name="amount" placeholder="Enter amount" required>
        <button type="submit">Submit Payment</button>
    </form>
    '''
def receive_payment():
    data = request.get_json()
    email = data.get('email')
    amount = data.get('amount')
    date = data.get('date')

    if not email or not amount or not date:
        return jsonify({"error": "Missing data"}), 400

    payment = Payment(email=email, amount=amount, date=date)
    db.session.add(payment)
    db.session.commit()

    return jsonify({"message": "Payment stored successfully"}), 201

if __name__ == "__main  __":
    app.run(host='0.0.0.0', port=5000)
