package org.orioncore.qt;

import android.os.Bundle;
import android.system.ErrnoException;
import android.system.Os;

import org.qtproject.qt5.android.bindings.QtActivity;

import java.io.File;

public class OrionQtActivity extends QtActivity
{
    @Override
    public void onCreate(Bundle savedInstanceState)
    {
        final File orionDir = new File(getFilesDir().getAbsolutePath() + "/.orion");
        if (!orionDir.exists()) {
            orionDir.mkdir();
        }

        super.onCreate(savedInstanceState);
    }
}
