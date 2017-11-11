package com.example.abcbr.osnumericalkeyboard.fragments;

import android.app.Activity;
import android.content.Context;
import android.os.Bundle;
import android.support.v4.app.Fragment;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.example.abcbr.osnumericalkeyboard.R;
import com.tapadoo.alerter.Alerter;

import java.util.Random;

/**
 * A simple {@link Fragment} subclass.
 * Use the {@link NumericKeyboard#newInstance} factory method to
 * create an instance of this fragment.
 */
public class NumericKeyboard extends Fragment implements View.OnClickListener{
    // TODO: Rename parameter arguments, choose names that match
    // the fragment initialization parameters, e.g. ARG_ITEM_NUMBER
    private static final String LAYOUT_ID = "param1";

    private static final int BUTTONS_IDS[] = {
            R.id.numpad0,
            R.id.numpad1,
            R.id.numpad2,
            R.id.numpad3,
            R.id.numpad4,
            R.id.numpad5,
            R.id.numpad6,
            R.id.numpad7,
            R.id.numpad8,
            R.id.numpad9};

    private View mView;

    private String randomPin = "";

    private String actualPin = "";

    private int layout_id;

    private int tapCounter = 0;

    private Button buttons[] = new Button[10];
    private Button changePin;

    private TextView txtvRandomPin;

    private EditText edtxtActualPin;

    public NumericKeyboard() {
        // Required empty public constructor
    }

    /**
     * Use this factory method to create a new instance of
     * this fragment using the provided parameters.
     *
     * @return A new instance of fragment N umericKeyboard.
     */
    // TODO: Rename and change types and number of parameters
    public static NumericKeyboard newInstance(int layoutId) {
        NumericKeyboard fragment = new NumericKeyboard();
        Bundle args = new Bundle();
        args.putInt(LAYOUT_ID, layoutId);
        fragment.setArguments(args);
        return fragment;
    }

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        if (getArguments() != null) {
            layout_id = getArguments().getInt(LAYOUT_ID);
        }
    }

    @Override
    public View onCreateView(LayoutInflater inflater, ViewGroup container,
                             Bundle savedInstanceState) {
        // Inflate the layout for this fragment
        mView = inflater.inflate(layout_id, container, false);

        for(int i = 0; i < 10; i++){
            buttons[i] = mView.findViewById(BUTTONS_IDS[i]);
            if(buttons[i] != null){
                buttons[i].setOnClickListener(this);
            } else {
                Log.e("Fragment" , "Fallo el boton");
            }
        }

        changePin = mView.findViewById(R.id.btnChangePin);

        txtvRandomPin = mView.findViewById(R.id.textViewPin);

        edtxtActualPin = mView.findViewById(R.id.edtxtActualPin);

        changePin = mView.findViewById(R.id.btnChangePin);
        changePin.setOnClickListener(this);

        return mView;
    }

    public String randomPin(){
        Random random = new Random();
        String randomPin = "";
        for(int i = 0; i < 6; i++){
            int digit = random.nextInt(10);
            randomPin = randomPin + digit;
        }
        return randomPin;
    }

    public void rightPin() {
        System.out.println("Pin correcto");
    }

    public void wrongPin() {
        System.out.println("Pin incorrecto");
    }

    @Override
    public void onClick(View view) {
        switch (view.getId()){
            case R.id.numpad0:
                if(tapCounter < 6){
                    actualPin = actualPin + "0";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.numpad1:
                if(tapCounter < 6){
                    actualPin = actualPin + "1";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.numpad2:
                if(tapCounter < 6){
                    actualPin = actualPin + "2";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.numpad3:
                if(tapCounter < 6){
                    actualPin = actualPin + "3";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.numpad4:
                if(tapCounter < 6){
                    actualPin = actualPin + "4";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.numpad5:
                if(tapCounter < 6){
                    actualPin = actualPin + "5";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.numpad6:
                if(tapCounter < 6){
                    actualPin = actualPin + "6";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.numpad7:
                if(tapCounter < 6){
                    actualPin = actualPin + "7";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.numpad8:
                if(tapCounter < 6){
                    actualPin = actualPin + "8";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.numpad9:
                if(tapCounter < 6){
                    actualPin = actualPin + "9";
                    edtxtActualPin.setText(actualPin);
                    tapCounter++;
                    if(tapCounter >= 6){
                        if(actualPin.compareTo(randomPin) == 0) {
                            rightPin();
                        } else {
                            wrongPin();
                        }
                    }
                }
                break;
            case R.id.btnChangePin:
                tapCounter = 0;
                actualPin = "";
                edtxtActualPin.setText("");
                randomPin = randomPin();
                txtvRandomPin.setText(randomPin);
                break;
            default:
                Log.e("Buttons listener", "Not button found");
        }
    }
}
