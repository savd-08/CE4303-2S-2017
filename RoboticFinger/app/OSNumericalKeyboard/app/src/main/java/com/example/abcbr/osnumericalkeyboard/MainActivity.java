package com.example.abcbr.osnumericalkeyboard;

import android.support.v4.app.Fragment;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;

import com.example.abcbr.osnumericalkeyboard.adapters.SectionsPagerAdapter;
import com.example.abcbr.osnumericalkeyboard.fragments.NumericKeyboard;

public class MainActivity extends AppCompatActivity{

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        ViewPager viewPager = (ViewPager) findViewById(R.id.view_pager);

        String titles[] = {"1 x 1", "2 x 2", "3 x 3"};

        Fragment fragments[] = {
                NumericKeyboard.newInstance(R.layout.fragment_numeric_keyboard),
                NumericKeyboard.newInstance(R.layout.fragment_numeric_keyboard2),
                NumericKeyboard.newInstance(R.layout.fragment_numeric_keyboard3)};

        SectionsPagerAdapter pagerAdapter = new SectionsPagerAdapter(getSupportFragmentManager(), fragments, titles);

        viewPager.setAdapter(pagerAdapter);
    }
}
